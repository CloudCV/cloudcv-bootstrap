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

                "src/framework/ImageSource.hpp",                
                "src/framework/ImageSource.cpp",

                "src/framework/Job.hpp",                
                "src/framework/Job.cpp",

                "src/framework/Async.hpp",
                "src/framework/Async.cpp",
                    
                "src/framework/NanCheck.hpp",
                "src/framework/NanCheck.cpp",
                
                "src/modules/common/ScopedTimer.hpp", 

                "src/modules/analyze/AnalyzeImageAlgorithm.cpp", 
                "src/modules/analyze/AnalyzeImageAlgorithm.hpp", 
                "src/modules/analyze/AnalyzeImageBinding.cpp", 

                "src/modules/buildInformation/buildInformation.cpp", 

                "src/modules/cameraCalibration/CameraCalibrationBinding.cpp", 
                "src/modules/cameraCalibration/CameraCalibrationAlgorithm.hpp", 
                "src/modules/cameraCalibration/CameraCalibrationAlgorithm.cpp", 
            ],

            'include_dirs': [
              'src/',
              "<!(node -e \"require('nan')\")",
              "<!(node -e \"require('native-opencv').include_dirs()\")"
            ],

            'libraries': [
                ">!(node -e \"require('native-opencv').libraries(false)\")"
            ],

            'configurations': {
                'Debug': {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'ExceptionHandling': '2',  # /EHsc
                        },
                    },
                },
                'Release': {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'ExceptionHandling': '2',  # /EHsc
                        },
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
