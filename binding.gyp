{
  'targets': [
    {
      'target_name': 'vxa-node',
      'sources': [ 'vxa-node.cc' ],
      'include_dirs': [
        'vision/kernel',
      ],
      'libraries': [
        '-L$(srcdir)/vision/lib',
        '-lVxa',
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
            'cflags_cc!': [
              '-fno-rtti',
            ],
            'cflags': [
              '-U_FORTIFY_SOURCE -frtti',
              '-Wno-delete-non-virtual-dtor',
              '-Wno-reorder',
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

