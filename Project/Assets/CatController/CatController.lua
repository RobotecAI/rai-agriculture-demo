local cat_control = {
    Properties = {
        AnimatedEntityId = { default = EntityId() },
        PathEntityId = { default = EntityId() },
        Speed = 1.0,
        ControlNamespace = { default = "tractor" },
        HeighOffset = { default = 0.25 }
    },
    Spline = nil,
    SplineTransform = nil,
    Progress = 0.0,
    Epsilon = 0.5,
    SpeedMultiplier = 1.0,
    CurrentTime = 0,
    IsRunning = false,
    CatControlTopic = "namespace/alert/flash"
}
function cat_control:OnActivate()
    self.tickBusHandler = TickBus.CreateHandler(self, 0)
    self.tickBusHandler:Connect()
    local tmp = self.CatControlTopic
    self.CatControlTopic = tmp:gsub("namespace", self.Properties.ControlNamespace)
    self.ros2BusHandler = SubscriberNotificationsBus.Connect(self, self.CatControlTopic)
    SubscriberRequestBus.Broadcast.SubscribeToStdMsgBool(self.CatControlTopic)
end

function cat_control:OnDeactivate()
    if self.ros2BusHandler ~= nil then
        self.ros2BusHandler:Disconnect()
    end
    if self.tickBusHandler ~= nil then
        self.tickBusHandler:Disconnect()
    end
end

function cat_control:OnStdMsgBool(value)
    Debug.Log("Received command " .. tostring(value))
    self.IsRunning = value
    -- if value then
    --     AnimGraphComponentRequestBus.Event.SetParameterFloat(self.Properties.AnimatedEntityId, 0, 1.0)
    -- else
    --     AnimGraphComponentRequestBus.Event.SetParameterFloat(self.Properties.AnimatedEntityId, 0, 0.0)
    -- end
end

function cat_control:OnTick(deltaTime, timePoint)
    assert(self.Properties.AnimatedEntityId ~= EntityId(), "No AnimatedEntityId set")
    assert(self.Properties.PathEntityId ~= EntityId(), "No path entity set")

    if deltaTime > 0.5 then
        return
    end

    if not self.IsRunning then
        return
    end

    self.CurrentTime = self.CurrentTime + deltaTime
    if self.Spline == nil then
        -- initialize
        self.Spline = SplineComponentRequestBus.Event.GetSpline(self.Properties.PathEntityId)
    else
        splineTransform = TransformBus.Event.GetWorldTM(self.Properties.PathEntityId)
        self.Progress = self.Progress + self.Properties.Speed * deltaTime * self.SpeedMultiplier
        local splineLen = self.Spline:GetSplineLength()
        if self.Progress + self.Epsilon < self.Spline:GetSplineLength() then
            local splineAddress = self.Spline:GetAddressByDistance(self.Progress)
            desiredVec3 = splineTransform * self.Spline:GetPosition(splineAddress)
            desiredVec3.z = desiredVec3.z + self.Properties.HeighOffset

            -- build base
            xAxis = self.Spline:GetTangent(splineAddress)
            zAxis = Vector3.CreateAxisZ()
            yAxis = Vector3.Cross(zAxis, xAxis)

            rotationMatrix = Matrix3x3.CreateFromColumns(xAxis, yAxis, zAxis)
            rotationMatrix = rotationMatrix:GetOrthogonalized()
            assert(rotationMatrix:IsOrthogonal(), "Rotation matrix is not orthonormal")
            rotationQuat = Quaternion.CreateFromMatrix3x3(rotationMatrix)

            TransformBus.Event.SetWorldTranslation(self.Properties.AnimatedEntityId, desiredVec3)
            TransformBus.Event.SetLocalRotationQuaternion(self.Properties.AnimatedEntityId, rotationQuat)
        end
    end
end

return cat_control
