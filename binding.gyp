{
  'variables': {
    'vision_root' : '<!(node -p process.env.osv)/software/src/master/src'
  },
  'targets': [
    {
      'target_name': 'vxanode',
      'sources': [
        'vxa-node.cc',
        'Vision_Evaluation_Gofer.cc',
        'Vision_Evaluation_Value.cc',
        'va_node.cc',
        'va_node_callback.cc',
        'va_node_entity.cc',
        'va_node_export.cc',
        'va_node_isolate.cc',
        'va_node_isolated.cc',
        'va_node_process.cc',
        'va_node_resolver.cc',
        'va_node_triggerable.cc'
      ],
      'include_dirs': [
        '<(vision_root)/kernel',
      ],
      'libraries': [
        '-lVcaMain',
        '-lVxa',
        '-lVsa',
        '-lVca',
        '-lV'
      ],
      
      ################
      'conditions': [
        ['OS=="linux"',
          {
            'include_dirs': [
              '<(vision_root)/M_Linux'
            ],
            'libraries': [
              '-lpthread',
              '-luuid',
            ],
            'cflags': [
              '-U_FORTIFY_SOURCE -frtti',
              '-Wno-delete-non-virtual-dtor',
              '-Wno-reorder',
            ],
            'cflags_cc!': [
              '-fno-rtti',
            ],
            'configurations' : {
              'Debug' : {
                'cflags' : [
                  '-L<(vision_root)/lib/dbg',
                ],
                'ldflags': [
                  '-L<(vision_root)/lib/dbg',
                  "'-Wl,-rpath=<(vision_root)/lib/dbg'",
                ],
              },
              'Release' : {
                'cflags' : [
                  '-L<(vision_root)/lib',
                ],
                'ldflags': [
                  '-L<(vision_root)/lib',
                  "'-Wl,-rpath=<(vision_root)/lib'",
                ],
              }
            },
          },
        ],
        ['OS=="mac"',
          {
            'include_dirs': [
              '<(vision_root)/M_Darwin'
            ],
            'libraries': [
              '-lpthread'
            ],
            'library_dirs': [
              '<(vision_root)/lib',
            ],
            'xcode_settings': {
              'GCC_ENABLE_CPP_RTTI': 'YES',
              'OTHER_CPLUSPLUSFLAGS': [
                '-Wno-delete-non-virtual-dtor',
                '-Wno-trigraphs',
                '-Wno-undefined-bool-conversion',
                '-Wno-format',
                '-Wno-reorder',
                '-Wno-logical-op-parentheses',
                '-Wno-switch',
                '-Wno-shift-op-parentheses',
                '-Wno-shift-negative-value',
                '-Wno-parentheses',
                '-Wno-deprecated-declarations',
                '-Wno-inconsistent-missing-override',
              ],
            },
          },
        ],
        ['OS=="win"', {
            'include_dirs': [
              '<(vision_root)/M_Windows'
            ],
            'configurations': {
              'Debug': {
                'library_dirs': [
                  '<(vision_root)/Debugx64'
                ],
                'msvs_settings': {
                  'VCCLCompilerTool': {
                    'RuntimeTypeInfo': 'true',
                    'ExceptionHandling': 1, # /EHsc
                  }
                }
              },
              'Release': {
                'library_dirs': [
                  '<(vision_root)/Releasex64'
                ],
                'msvs_settings': {
                  'VCCLCompilerTool': {
                    'RuntimeTypeInfo': 'true',
                    'ExceptionHandling': 1, # /EHsc
                  }
                }
              }
            }
          }
        ]
      ]
      ################
    }
  ]
}

