{
  'targets': [
    {
      'target_name': 'vxanode',
      'sources': [
        'vxa-node.cc',
        'Vision_Evaluation_Gofer.cc',
        'Vision_Evaluation_Value.cc'
      ],
      'include_dirs': [
        'vision/kernel',
      ],
      'libraries': [
        '-Wl,-rpath=./vision/lib',
        '-L$(srcdir)/vision/lib',
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
            'ldflags': [
            ],
          },
        ],
      ],
      ################
    },
  ],
}

