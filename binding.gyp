{
  'targets': [
    {
      'target_name': 'vxanode',
      'sources': [
        'vxa-node.cc',
        'Vision_Evaluation_Gofer.cc',
        'Vision_Evaluation_Value.cc',
        'va_node.cc',
        'va_node_process.cc',
        'va_node_isolate.cc',
        'va_node_export.cc'
      ],
      'include_dirs': [
        'vision/kernel',
      ],
      'libraries': [
        '-lVxa',
        '-lVsa',
        '-lVca',
        '-lV',
        '-lpthread',
        '-luuid',
      ],
      
      ################
      'conditions': [
        ['OS=="linux"',
          {
            'include_dirs': [
              'vision/M_Linux'
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
                  '-L$(srcdir)/vision/lib/dbg',
                ],
                'ldflags': [
                  '-L$(srcdir)/vision/lib/dbg',
                  "'-Wl,-rpath=$$ORIGIN/../../vision/lib/dbg'",
                ],
              },
              'Release' : {
                'cflags' : [
                  '-L$(srcdir)/vision/lib',
                ],
                'ldflags': [
                  '-L$(srcdir)/vision/lib',
                   "'-Wl,-rpath=$$ORIGIN/../../vision/lib'",
                ],
              }
            },
          },
        ],
        ['OS=="mac"',
          {
            'include_dirs': [
              'vision/M_Darwin'
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
      ],
      ################
    },
  ],
}

