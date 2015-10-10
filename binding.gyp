{

    'target_defaults': {
        'default_configuration': 'Release',

    },


    'targets': [
        {
            'target_name': "cloudcv",

            'sources': [ 
                "src/cloudcv.cpp", 
                "src/cloudcv.hpp",

                "src/framework/Logger.hpp",                
                "src/framework/ScopedTimer.hpp",                

                "src/framework/marshal/marshal.hpp",
                "src/framework/marshal/opencv.hpp",                

                "src/framework/ImageView.hpp",                
                "src/framework/ImageView.cpp",

                "src/framework/Job.hpp",                
                "src/framework/Job.cpp",

                "src/framework/Algorithm.hpp",
                "src/framework/Algorithm.cpp",

                "src/framework/AlgorithmInfo.hpp",
                "src/framework/AlgorithmInfo.cpp",

                "src/framework/Argument.hpp",
                "src/framework/Argument.cpp",

                "src/framework/AlgorithmExceptions.hpp",
                "src/framework/AlgorithmExceptions.cpp",
                
                "src/modules/HoughLines.cpp",
                "src/modules/HoughLines.hpp",

                "src/modules/IntegralImage.hpp",
                "src/modules/IntegralImage.cpp"
            ],

            'include_dirs': [
              'src/',
              "<!(node -e \"require('nan')\")",
              "<!(node -e \"require('nan-check')\")",
              "<!(node -e \"require('native-opencv').include_dirs()\")"
            ],

            'libraries': [
                ">!(node -e \"require('native-opencv').libraries(true)\")"
            ],

            'configurations': {
                'Debug': {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'RuntimeTypeInfo': 'true',
                            'ExceptionHandling': '2',  # /EHsc
                        },
                        'VCLinkerTool': {
                            'GenerateDebugInformation': 'true',
                        }
                    },
                },
                'Release': {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'RuntimeTypeInfo': 'true',
                            'ExceptionHandling': '2',  # /EHsc
                        },
                        'VCLinkerTool': {
                            'GenerateDebugInformation': 'true',
                        }
                    },
                },
            },

            'target_conditions': [
                
                ['OS=="win"', {

                }],
                ['OS=="mac"', {
                
                    'defines': [
                        'TARGET_PLATFORM_MAC',
                    ],

                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'GCC_ENABLE_CPP_RTTI': 'YES',
                        'OTHER_CFLAGS': [ '-g', '-mmacosx-version-min=10.7', '-std=c++11', '-stdlib=libc++', '-O3', '-Wall' ],
                        'OTHER_CPLUSPLUSFLAGS': [ '-g', '-mmacosx-version-min=10.7', '-std=c++11', '-stdlib=libc++', '-O3', '-Wall' ]
                    }
                }],

                
                ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
                
                    'defines': [
                        'TARGET_PLATFORM_LINUX',
                    ],

                    'libraries!': [ '-undefined dynamic_lookup' ],

                    'cflags_cc!': [ '-fno-exceptions', '-fno-rtti' ],
                    "cflags": [ '-std=c++11', '-fexceptions', '-frtti' ],                    
                }],

                ['OS=="win"', {
                    'defines': [
                        'TARGET_PLATFORM_WINDOWS',
                    ]             
                }]

            ]
        }
    ]
}
