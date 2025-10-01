{
    "Source" : "./Billboard_ForwardPass.azsl",

    "DepthStencilState" :
    {
        "Depth" :
        {
            "Enable" : true,
            "CompareFunc" : "GreaterEqual",
            "writeMask": "Zero"
        }
    },
    "GlobalTargetBlendState" :
    {
        "Enable" : true,
        "BlendSource" : "One",
        "BlendDest" : "AlphaSourceInverse",
        "BlendAlphaOp" : "Add"
    },
    "ProgramSettings":
    {
      "EntryPoints":
      [
        {
          "name": "Billboard_ForwardPassVS",
          "type": "Vertex"
        },
        {
          "name": "Billboard_ForwardPassPS",
          "type": "Fragment"
        }
      ]
    },

    "DrawList" : "forward"
}
