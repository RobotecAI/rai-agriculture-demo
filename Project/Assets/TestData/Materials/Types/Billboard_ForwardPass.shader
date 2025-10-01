{
    "Source" : "./Billboard_ForwardPass.azsl",

    "DepthStencilState" :
    {
        "Depth" :
        {
            "Enable" : true,
            "CompareFunc" : "GreaterEqual"
        },
        "Stencil" :
        {
            "Enable" : true,
            "ReadMask" : "0x00",
            "WriteMask" : "0xFF",
            "FrontFace" :
            {
                "Func" : "Always",
                "DepthFailOp" : "Keep",
                "FailOp" : "Keep",
                "PassOp" : "Replace"
            }
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
