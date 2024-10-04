local RobotStates = { WAITING = 1, DRIVING = 2, REVERSE = 3, CHANGING_PATH = 4, FINISHED = 5 }

local tractor_control =
{
	Properties =
	{
		Tractor = {
			TractorEntityId = { default = EntityId() },
			Speed = 1.0,
			SteeringGain = 10.0,
			LookAhead = 1.0,
			TractorForwardAxis = { default = Vector3.ConstructFromValues(0.0, 1.0, 0.0) },
			TractorRightAxis = { default = Vector3.ConstructFromValues(1.0, 0.0, 0.0) },
			Topic = "ackermann_vel"
		},
		ControlNamespace = { default = "tractor" },
		Debug = true,
		StartupDelay = { default = 10 },
		Paths = {
			Path1 = { default = EntityId() },
			Path2 = { default = EntityId() },
			Path3 = { default = EntityId() }
		},
		Collisions = {
			Collision1 = { default = EntityId() },
			Collision2 = { default = EntityId() },
			Collision3 = { default = EntityId() }
		}
	},
	RobotState = RobotStates.DRIVING,
	Direction = 1.0,
	RobotControlTopic = "namespace/control/move",
	RobotStateTopic = "namespace/state",
	RobotVelTopic = "namespace/",
	ObstacleDetected = false,
	SplineComponent = nil,
	PathEntity = nil,
	CurrentPath = 0,
	CurrentTime = 0
}

function tractor_control:OnActivate()
	assert(self.Properties.Tractor.TractorEntityId ~= EntityId(), "No tractor entity set")
	assert(self.Properties.Paths.Path1 ~= EntityId(), "No path entity set")
	self.tickBusHandler = TickBus.Connect(self, 0)
	local tmp = self.RobotControlTopic
	self.RobotControlTopic = tmp:gsub("namespace", self.Properties.ControlNamespace)
	self.ros2BusHandler = SubscriberNotificationsBus.Connect(self, self.RobotControlTopic)
	SubscriberRequestBus.Broadcast.SubscribeToStdMsgString(self.RobotControlTopic)
	tmp = self.RobotStateTopic
	self.RobotStateTopic = tmp:gsub("namespace", self.Properties.ControlNamespace)
	tmp = self.RobotVelTopic
	self.RobotVelTopic = tmp:gsub("namespace", self.Properties.ControlNamespace) .. self.Properties.Tractor.Topic
end

function tractor_control:OnDeactivate()
	if self.ros2BusHandler ~= nil then
		self.ros2BusHandler:Disconnect()
	end
	if self.tickBusHandler ~= nil then
		self.tickBusHandler:Disconnect()
	end
end

function tractor_control:CheckDetections(tractorPosition)
	for _, object in ipairs(self.CollisionsFlattened) do
		local objectPosition = TransformBus.Event.GetWorldTranslation(object)
		local distance = objectPosition - tractorPosition
		if distance:GetLength() < 5.0 then
			if not self.ObstacleDetected then
				self.ObstacleDetected = true
				self.RobotState = RobotStates.WAITING
				self.Direction = 0.0
				Debug.Log("Stopped tractor to avoid the collision")
				PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "WAITING")
				PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "OBSTACLE")
			end
			return
		end
		if self.Properties.Debug then
			DebugDrawRequestBus.Broadcast.DrawTextAtLocation(objectPosition + Vector3.CreateAxisZ(2),
				tostring(distance:GetLength()), Color.ConstructFromValues(0, 0, 255, 255), 0)
		end
	end
	self.ObstacleDetected = false
end

function tractor_control:GetNextSpline()
	self.CurrentPath = self.CurrentPath + 1
	assert(self.CurrentPath <= self.TotalPaths)
	self.PathEntity = self.PathsFlattened[self.CurrentPath]
	self.SplineComponent = SplineComponentRequestBus.Event.GetSpline(self.PathEntity)
	Debug.Log("Using spline " .. self.CurrentPath .. " out of " .. self.TotalPaths)
end

function tractor_control:MoveTractor()
	local tractorPosition = TransformBus.Event.GetWorldTranslation(self.Properties.Tractor.TractorEntityId)
	assert(tractorPosition ~= nil, "tractorPosition is missing")
	local tractorPose = TransformBus.Event.GetWorldTM(self.Properties.Tractor.TractorEntityId)
	assert(tractorPose ~= nil, "tractorPose is missing")

	self.CheckDetections(self, tractorPosition)

	tractorPoseInv = Transform.Clone(tractorPose)
	Transform.Invert(tractorPoseInv)

	local lookAhead = self.Properties.Tractor.LookAhead * self.Direction
	probePosition = tractorPosition +
		Transform.TransformVector(tractorPose, self.Properties.Tractor.TractorForwardAxis * lookAhead)
	assert(probePosition ~= nil, "probePosition is missing")

	local splineTransform = TransformBus.Event.GetWorldTM(self.PathEntity)
	splineTransformInv = Transform.Clone(splineTransform)
	Transform.Invert(splineTransformInv)

	probePositionInSplineFrame = splineTransformInv * probePosition

	local nearestAddress = self.SplineComponent:GetNearestAddressPosition(probePositionInSplineFrame)
	local nearestPosition = self.SplineComponent:GetPosition(nearestAddress.splineAddress)

	local nearestPositionWorld = splineTransform * nearestPosition
	nearestPositionTractorFrame = tractorPoseInv * nearestPositionWorld

	crossTrackError = Vector3.Dot(nearestPositionTractorFrame, self.Properties.Tractor.TractorRightAxis)
	lateralError = Vector3.Dot(nearestPositionTractorFrame, self.Properties.Tractor.TractorForwardAxis) - lookAhead

	local steeringAngle = -self.Properties.Tractor.SteeringGain * crossTrackError;
	local speed = self.Properties.Tractor.Speed * self.Direction
	-- reduce steering for large distances
	if lateralError > 1.0 then
		steeringAngle = steeringAngle / lateralError
	end
	PublisherRequestBus.Broadcast.PublishAckermannDriveMsg(self.RobotVelTopic, steeringAngle, 0.0, speed, 0.0,
		0.0)

	if self.Properties.Debug then
		DebugDrawRequestBus.Broadcast.DrawSphereAtLocation(nearestPositionWorld, 0.2,
			Color.ConstructFromValues(255, 0, 0, 255), 0)
		DebugDrawRequestBus.Broadcast.DrawTextAtLocation(nearestPositionWorld + Vector3.CreateAxisZ(1),
			'crosstrack ' .. tostring(crossTrackError), Color.ConstructFromValues(255, 0, 0, 255), 0)
		DebugDrawRequestBus.Broadcast.DrawTextAtLocation(nearestPositionWorld + Vector3.CreateAxisZ(2),
			'lateral ' .. tostring(lateralError), Color.ConstructFromValues(0, 255, 0, 255), 0)
	end

	return { crossTrackError, lateralError }
end

function tractor_control:OnStdMsgString(command)
	Debug.Log("Received command: " .. command)
	if command == "CONTINUE" then
		self.RobotState = RobotStates.DRIVING
		self.Direction = 1.0
		Debug.Log("Set DRIVING after received command: " .. command)
		PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "DRIVING")
	elseif command == "STOP" then
		self.RobotState = RobotStates.WAITING
		self.Direction = 0.0
		Debug.Log("Set WAITING after received command: " .. command)
		PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "WAITING")
	elseif command == "REPLAN" then
		self.RobotState = RobotStates.REVERSE
		self.Direction = -1.0
		PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "REVERSE")
		Debug.Log("Set REVERSE after received command: " .. command)
	elseif command == "STATE" then
		for k, v in pairs(RobotStates) do
			if self.RobotState == v then
				PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, k)
				Debug.Log("Requested state information: " .. k)
			end
		end
	else
		Debug.Log("Cannot parse the command: " .. command)
	end
end

function tractor_control:OnTick(deltaTime, timePoint)
	if deltaTime > 0.25 then
		return
	end

	-- startup delay
	self.CurrentTime = self.CurrentTime + deltaTime
	if self.CurrentTime < self.Properties.StartupDelay then
		return
	end

	if self.RobotState == RobotStates.WAITING or self.RobotState == RobotStates.FINISHED then
		return
	end

	if self.SplineComponent == nil then
		-- initialize
		self.TotalPaths = 3
		self.PathsFlattened = { self.Properties.Paths.Path1, self.Properties.Paths.Path2, self.Properties.Paths.Path3 }
		self.CollisionsFlattened = { self.Properties.Collisions.Collision1, self.Properties.Collisions.Collision2, self
			.Properties.Collisions.Collision3 }
		self.GetNextSpline(self)
		PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "DRIVING")
	else
		-- move the tractor and check for errors to modify the state machine if needed
		local positionErrors = self.MoveTractor(self)
		crossTrackError = positionErrors[1]
		lateralError = positionErrors[2]

		-- state machine
		if self.RobotState == RobotStates.DRIVING then
			if lateralError < -0.5 then
				-- reached the end of the current spline; switch spline or terminate
				if self.CurrentPath < self.TotalPaths then
					self.RobotState = RobotStates.CHANGING_PATH
					self.GetNextSpline(self)
					PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic,
						"CHANGING_PATH")
					Debug.Log("Robot CHANGING_PATH")
				else
					self.RobotState = RobotStates.FINISHED
					self.Direction = 0
					PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "FINISHED")
					Debug.Log("Robot FINISHED")
				end
			end
		elseif self.RobotState == RobotStates.REVERSE then
			if lateralError > 0.5 then
				-- reached the beginning of the aborted spline; switch spline (and change driving direction) or terminate
				if self.CurrentPath < self.TotalPaths then
					self.RobotState = RobotStates.CHANGING_PATH
					self.GetNextSpline(self)
					self.Direction = 1.0
					PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic,
						"CHANGING_PATH")
					Debug.Log("Robot CHANGING_PATH")
				else
					self.RobotState = RobotStates.FINISHED
					self.Direction = 0
					PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "FINISHED")
					Debug.Log("Robot FINISHED")
				end
			end
		elseif self.RobotState == RobotStates.CHANGING_PATH then
			if math.abs(lateralError) < 0.5 and math.abs(crossTrackError) < 0.5 then
				-- reached the new spline
				self.RobotState = RobotStates.DRIVING
				PublisherRequestBus.Broadcast.PublishStdMsgString(self.RobotStateTopic, "DRIVING")
				Debug.Log("Robot DRIVING")
			end
		end
	end
end

return tractor_control
