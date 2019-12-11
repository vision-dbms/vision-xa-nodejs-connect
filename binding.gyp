{
  'variables': {
    'vision_src' : 'deps/vision/src'
  },
  'target_defaults': {
    'include_dirs': [
      '<(vision_src)/kernel',
    ],
    'target_conditions': [
      ['OS=="linux"', {
        'include_dirs': [
          '<(vision_src)/M_Linux'
        ],
        'libraries': [
          '-lpthread',
          '-luuid',
        ],
        'cflags': [
          '-U_FORTIFY_SOURCE -frtti -fexceptions',
          '-Wno-delete-non-virtual-dtor',
          '-Wno-reorder',
          '-Wno-char-subscripts',
          '-Wno-parentheses',
          '-Wno-sign-compare',
          '-Wno-maybe-uninitialized',
          '-Wno-switch',
          '-Wno-unused-variable',
          '-Wno-unused-function',
          '-Wno-extra',
          '-Wno-clobbered',
        ],
        'cflags_cc!': [
          '-fno-rtti', '-fno-exceptions'
        ],
        'ldflags': [
           "-Wl,-rpath,'$$ORIGIN'"
        ],
      }],
      ['OS=="mac"', {
        'include_dirs': [
          '<(vision_src)/M_Darwin'
        ],
        'libraries': [
          '-lpthread',
        ],
        'xcode_settings': {
          'GCC_ENABLE_CPP_RTTI': 'YES',
          'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          'LD_DYLIB_INSTALL_NAME': '@loader_path/>(_target_name).dylib',
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
	    '-Wno-char-subscripts',
	    '-Wno-sign-compare',
	    '-Wno-unused-private-field',
	    '-Wno-unused-variable',
	    '-Wno-unused-function',
          ],
        },
      }],
      ['OS=="win"', {
        'include_dirs': [
          '<(vision_src)/M_Windows'
        ],
        'msvs_disabled_warnings': [
           '4018',
           '4244',
           '4477',
           '4700',
           '4910',
        ],
        'configurations': {
          'Debug': {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'RuntimeTypeInfo': 'true',
                'ExceptionHandling': 1, # /EHsc
              }
            }
          },
          'Release': {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'RuntimeTypeInfo': 'true',
                'ExceptionHandling': 1, # /EHsc',
                'InlineFunctionExpansion': 1,
              }
            }
          }
        }
      }]
    ]
  },
  'targets': [
    ################
    ####  vxanode
    ################
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
        'va_node_function_template.cc',
        'va_node_import.cc',
        'va_node_isolate.cc',
        'va_node_isolated.cc',
        'va_node_object_template.cc',
        'va_node_process.cc',
        'va_node_template.cc',
        'va_node_triggerable.cc'
      ],
      'dependencies': [
        'VcaMain',
        'Vxa',
        'Vsa',
        'Vca',
        'V'
      ],
    },

    ################
    ####  V
    ################
    {
      'target_name' : 'V',
      'type' : 'shared_library',
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'V_EXPORTS'
          ]
        }]
      ],
      'sources' : [
        '<(vision_src)/kernel/gopt.cpp',
        '<(vision_src)/kernel/V.cpp',
        '<(vision_src)/kernel/V_VAdaptiveAllocator.cpp',
        '<(vision_src)/kernel/V_VAddin.cpp',
        '<(vision_src)/kernel/V_VAllocator.cpp',
        '<(vision_src)/kernel/V_VApplicationLog.cpp',
        '<(vision_src)/kernel/V_VCamLog.cpp',
        '<(vision_src)/kernel/V_VArgList.cpp',
        '<(vision_src)/kernel/V_VAtom.cpp',
        '<(vision_src)/kernel/V_VBlob.cpp',
        '<(vision_src)/kernel/V_VCommandLine.cpp',
        '<(vision_src)/kernel/V_VCondvar.cpp',
        '<(vision_src)/kernel/V_VCOS.cpp',
        '<(vision_src)/kernel/V_VFifoLite.cpp',
        '<(vision_src)/kernel/V_VGSID.cpp',
        '<(vision_src)/kernel/V_VLock.cpp',
        '<(vision_src)/kernel/V_VLogger.cpp',
        '<(vision_src)/kernel/V_VMM.cpp',
        '<(vision_src)/kernel/V_VManagedThread.cpp',
        '<(vision_src)/kernel/V_VMutex.cpp',
        '<(vision_src)/kernel/V_VNullock.cpp',
        '<(vision_src)/kernel/V_VRTTI.cpp',
        '<(vision_src)/kernel/V_VRandom.cpp',
        '<(vision_src)/kernel/V_VSSID.cpp',
        '<(vision_src)/kernel/V_VScheduler.cpp',
        '<(vision_src)/kernel/V_VScopedResource.cpp',
        '<(vision_src)/kernel/V_VSemaphore.cpp',
        '<(vision_src)/kernel/V_VSpinlock.cpp',
        '<(vision_src)/kernel/V_VThread.cpp',
        '<(vision_src)/kernel/V_VThreadSpecific.cpp',
        '<(vision_src)/kernel/V_VTime.cpp',
        '<(vision_src)/kernel/V_VTwiddler.cpp',
        '<(vision_src)/kernel/V_VUnmanagedThread.cpp',
        '<(vision_src)/kernel/VdbNetwork.cpp',
        '<(vision_src)/kernel/Vk.cpp',
        '<(vision_src)/kernel/VkBuffer.cpp',
        '<(vision_src)/kernel/VkLongUnsigneds.cpp',
        '<(vision_src)/kernel/VkMemory.cpp',
        '<(vision_src)/kernel/VkRegExp.cpp',
        '<(vision_src)/kernel/VkRunTimes.cpp',
        '<(vision_src)/kernel/VkStatus.cpp',
        '<(vision_src)/kernel/VkUUID.cpp',
        '<(vision_src)/kernel/VReferenceable.cpp',
        '<(vision_src)/kernel/VRunTimeType.cpp',
        '<(vision_src)/kernel/VSimpleFile.cpp',
        '<(vision_src)/kernel/VStaticTransient.cpp',
        '<(vision_src)/kernel/V_VString.cpp',
        '<(vision_src)/kernel/VTransient.cpp',
        '<(vision_src)/kernel/VTransientServices.cpp',
        '<(vision_src)/kernel/cam.cpp',
      ]
    },

    ################
    ####  Vca
    ################
    {
      'target_name': 'Vca',
      'type': 'shared_library',
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'VCA_EXPORTS'
          ]
        }]
      ],
      'dependencies': [
        'V'
      ],
      'sources': [
        '<(vision_src)/kernel/IVReceiver.cpp',
        '<(vision_src)/kernel/IVUnknown.cpp',
        '<(vision_src)/kernel/Vca.cpp',
        '<(vision_src)/kernel/Vca_IAckReceiver.cpp',
        '<(vision_src)/kernel/Vca_IActivity.cpp',
        '<(vision_src)/kernel/Vca_IActivityProxy.cpp',
        '<(vision_src)/kernel/Vca_IActivitySink.cpp',
        '<(vision_src)/kernel/Vca_IActivitySource.cpp',
        '<(vision_src)/kernel/Vca_IApplication.cpp',
        '<(vision_src)/kernel/Vca_IBinding.cpp',
        '<(vision_src)/kernel/Vca_IBSClient.cpp',
        '<(vision_src)/kernel/Vca_IBSConsumer.cpp',
        '<(vision_src)/kernel/Vca_IBSConsumerClient.cpp',
        '<(vision_src)/kernel/Vca_IBSProducer.cpp',
        '<(vision_src)/kernel/Vca_IBSProducerClient.cpp',
        '<(vision_src)/kernel/Vca_IClient.cpp',
        '<(vision_src)/kernel/Vca_IConnection.cpp',
        '<(vision_src)/kernel/Vca_IConnectionFactory.cpp',
        '<(vision_src)/kernel/Vca_IConnectionRequest.cpp',
        '<(vision_src)/kernel/Vca_IDataSource.cpp',
        '<(vision_src)/kernel/Vca_IDevice.cpp',
        '<(vision_src)/kernel/Vca_IDirectory.cpp',
        '<(vision_src)/kernel/Vca_IDirectoryEntry.cpp',
        '<(vision_src)/kernel/Vca_IError.cpp',
        '<(vision_src)/kernel/Vca_IEvent.cpp',
        '<(vision_src)/kernel/Vca_IEventSink.cpp',
        '<(vision_src)/kernel/Vca_IGetter.cpp',
        '<(vision_src)/kernel/Vca_IGetter_Ex2.cpp',
        '<(vision_src)/kernel/Vca_IInfoServer.cpp',
        '<(vision_src)/kernel/Vca_IListener.cpp',
        '<(vision_src)/kernel/Vca_IListenerClient.cpp',
        '<(vision_src)/kernel/Vca_IListenerFactory.cpp',
        '<(vision_src)/kernel/Vca_INamespace.cpp',
        '<(vision_src)/kernel/Vca_INeedYou.cpp',
        '<(vision_src)/kernel/Vca_IPassiveCall.cpp',
        '<(vision_src)/kernel/Vca_IPassiveCallback.cpp',
        '<(vision_src)/kernel/Vca_IPassiveCallback_Gofers.cpp',
        '<(vision_src)/kernel/Vca_IPassiveClient.cpp',
        '<(vision_src)/kernel/Vca_IPassiveConnector.cpp',
        '<(vision_src)/kernel/Vca_IPassiveConnector_Gofers.cpp',
        '<(vision_src)/kernel/Vca_IPassiveServer.cpp',
        '<(vision_src)/kernel/Vca_IPassiveServer_Gofers.cpp',
        '<(vision_src)/kernel/Vca_IPeek.cpp',
        '<(vision_src)/kernel/Vca_IPeer.cpp',
        '<(vision_src)/kernel/Vca_IPeer_Ex1.cpp',
        '<(vision_src)/kernel/Vca_IPeer_Ex2.cpp',
        '<(vision_src)/kernel/Vca_IPipeFactory.cpp',
        '<(vision_src)/kernel/Vca_IPipeSource.cpp',
        '<(vision_src)/kernel/Vca_IPipeSource_Gofers.cpp',
        '<(vision_src)/kernel/Vca_IPipeSourceClient.cpp',
        '<(vision_src)/kernel/Vca_IProcess.cpp',
        '<(vision_src)/kernel/Vca_IProcessFactory.cpp',
        '<(vision_src)/kernel/Vca_IProcessFactoryClient.cpp',
        '<(vision_src)/kernel/Vca_IProcessInfo.cpp',
        '<(vision_src)/kernel/Vca_IProcessInfoSink.cpp',
        '<(vision_src)/kernel/Vca_IRequest.cpp',
        '<(vision_src)/kernel/Vca_IResource.cpp',
        '<(vision_src)/kernel/Vca_IResourceClient.cpp',
        '<(vision_src)/kernel/Vca_IRexecConnectionFactory.cpp',
        '<(vision_src)/kernel/Vca_IRoleProvider.cpp',
        '<(vision_src)/kernel/Vca_IRoleProvider2.cpp',
        '<(vision_src)/kernel/Vca_IServerApplication.cpp',
        '<(vision_src)/kernel/Vca_ISSHConnectionFactory.cpp',
        '<(vision_src)/kernel/Vca_IStdPipeSource.cpp',
        '<(vision_src)/kernel/Vca_IStdPipeSourceClient.cpp',
        '<(vision_src)/kernel/Vca_ITrigger.cpp',
        '<(vision_src)/kernel/Vca_Registry.cpp',
        '<(vision_src)/kernel/Vca_Registry_IAccess.cpp',
        '<(vision_src)/kernel/Vca_Registry_IContact.cpp',
        '<(vision_src)/kernel/Vca_Registry_IUpdate.cpp',
        '<(vision_src)/kernel/Vca_Registry_IUpdater.cpp',
        '<(vision_src)/kernel/Vca_Registry_IAuthority.cpp',
        '<(vision_src)/kernel/Vca_Registry_IRegistration.cpp',
        '<(vision_src)/kernel/Vca_Registry_IRegistry.cpp',
        '<(vision_src)/kernel/Vca_Registry_VAuthority.cpp',
        '<(vision_src)/kernel/Vca_Registry_VRegistry.cpp',
        '<(vision_src)/kernel/Vca_VActivity.cpp',
        '<(vision_src)/kernel/Vca_VActivityFilter.cpp',
        '<(vision_src)/kernel/Vca_VActivityModel.cpp',
        '<(vision_src)/kernel/Vca_VActivityMonitor.cpp',
        '<(vision_src)/kernel/Vca_VActivityRelay.cpp',
        '<(vision_src)/kernel/Vca_VActivitySink.cpp',
        '<(vision_src)/kernel/Vca_VBS.cpp',
        '<(vision_src)/kernel/Vca_VBSConnector.cpp',
        '<(vision_src)/kernel/Vca_VBSConsumer.cpp',
        '<(vision_src)/kernel/Vca_VBSProducer.cpp',
        '<(vision_src)/kernel/Vca_VBSProducerBitBucket.cpp',
        '<(vision_src)/kernel/Vca_VBSManager.cpp',
        '<(vision_src)/kernel/Vca_VcaConnection.cpp',
        '<(vision_src)/kernel/Vca_VcaCredentialsProvider.cpp',
        '<(vision_src)/kernel/Vca_VcaDirectoryBuilder.cpp',
        '<(vision_src)/kernel/Vca_VcaGofer.cpp',
        '<(vision_src)/kernel/Vca_VcaListener.cpp',
        '<(vision_src)/kernel/Vca_VCallbackTicket.cpp',
        '<(vision_src)/kernel/Vca_VcaOffer.cpp',
        '<(vision_src)/kernel/Vca_VcaOID.cpp',
        '<(vision_src)/kernel/Vca_VcaOIDL.cpp',
        '<(vision_src)/kernel/Vca_VcaOIDR.cpp',
        '<(vision_src)/kernel/Vca_VcaPeer.cpp',
        '<(vision_src)/kernel/Vca_VcaPeerCharacteristics.cpp',
        '<(vision_src)/kernel/Vca_VcaProxyFacilitator.cpp',
        '<(vision_src)/kernel/Vca_VcaRouteStatistics.cpp',
        '<(vision_src)/kernel/Vca_VcaSelf.cpp',
        '<(vision_src)/kernel/Vca_VcaSerializer.cpp',
        '<(vision_src)/kernel/Vca_VcaSerializer_.cpp',
        '<(vision_src)/kernel/Vca_VcaSite.cpp',
        '<(vision_src)/kernel/Vca_VcaThreadState.cpp',
        '<(vision_src)/kernel/Vca_VcaTransport.cpp',
        '<(vision_src)/kernel/Vca_VClassInfoHolder.cpp',
        '<(vision_src)/kernel/Vca_VCohort.cpp',
        '<(vision_src)/kernel/Vca_VConnection.cpp',
        '<(vision_src)/kernel/Vca_VConnectionFactory.cpp',
        '<(vision_src)/kernel/Vca_VConnectionPipeSource.cpp',
        '<(vision_src)/kernel/Vca_VConnectionRequest.cpp',
        '<(vision_src)/kernel/Vca_VDevice.cpp',
        '<(vision_src)/kernel/Vca_VDeviceFactory.cpp',
        '<(vision_src)/kernel/Vca_VDeviceImplementation.cpp',
        '<(vision_src)/kernel/Vca_VDeviceManager.cpp',
        '<(vision_src)/kernel/Vca_VDirectory.cpp',
        '<(vision_src)/kernel/Vca_VDirectoryBuilder.cpp',
        '<(vision_src)/kernel/Vca_VEvent.cpp',
        '<(vision_src)/kernel/Vca_VFDIODeviceImplementation.cpp',
        '<(vision_src)/kernel/Vca_VFilterDeviceFactory.cpp',
        '<(vision_src)/kernel/Vca_VFilterDeviceImplementation.cpp',
        '<(vision_src)/kernel/Vca_VGofer.cpp',
        '<(vision_src)/kernel/Vca_VInterfaceQuery.cpp',
        '<(vision_src)/kernel/Vca_VInterfaceMember.cpp',
        '<(vision_src)/kernel/Vca_VLineGrabber.cpp',
        '<(vision_src)/kernel/Vca_VListener.cpp',
        '<(vision_src)/kernel/Vca_VListenerFactory.cpp',
        '<(vision_src)/kernel/Vca_VLoopbackDeviceFactory.cpp',
        '<(vision_src)/kernel/Vca_VLoopbackDeviceImplementation.cpp',
        '<(vision_src)/kernel/Vca_VManager.cpp',
        '<(vision_src)/kernel/Vca_VMessage.cpp',
        '<(vision_src)/kernel/Vca_VMessageManager.cpp',
        '<(vision_src)/kernel/Vca_VMessageScheduler.cpp',
        '<(vision_src)/kernel/Vca_VNotifier.cpp',
        '<(vision_src)/kernel/Vca_VNullDeviceFactory.cpp',
        '<(vision_src)/kernel/Vca_VNullDeviceImplementation.cpp',
        '<(vision_src)/kernel/Vca_VOutputSequencer.cpp',
        '<(vision_src)/kernel/Vca_VPassiveAgent.cpp',
        '<(vision_src)/kernel/Vca_VPassiveCallback.cpp',
        '<(vision_src)/kernel/Vca_VPassiveConnector.cpp',
        '<(vision_src)/kernel/Vca_VPassiveDirectiveAgent.cpp',
        '<(vision_src)/kernel/Vca_VPassiveProcessAgent.cpp',
        '<(vision_src)/kernel/Vca_VPipeSource.cpp',
        '<(vision_src)/kernel/Vca_VProcess.cpp',
        '<(vision_src)/kernel/Vca_VProcessFactory.cpp',
        '<(vision_src)/kernel/Vca_VPublication.cpp',
        '<(vision_src)/kernel/Vca_VRexecConnectionFactory.cpp',
        '<(vision_src)/kernel/Vca_VRoleHolder.cpp',
        '<(vision_src)/kernel/Vca_VRolePlayer.cpp',
        '<(vision_src)/kernel/Vca_VSiteInfo.cpp',
        '<(vision_src)/kernel/Vca_VSSHConnectionFactory.cpp',
        '<(vision_src)/kernel/Vca_VSSHDeviceImplementation.cpp',
        '<(vision_src)/kernel/Vca_VStatus.cpp',
        '<(vision_src)/kernel/Vca_VStdPipeSource.cpp',
        '<(vision_src)/kernel/Vca_VStockDeviceFactory.cpp',
        '<(vision_src)/kernel/Vca_VStreamSink.cpp',
        '<(vision_src)/kernel/Vca_VStreamSource.cpp',
        '<(vision_src)/kernel/Vca_VSubscriptionManager.cpp',
        '<(vision_src)/kernel/Vca_VTimer.cpp',
        '<(vision_src)/kernel/Vca_VTriggerSet.cpp',
        '<(vision_src)/kernel/Vca_VTriggerableTicket.cpp',
        '<(vision_src)/kernel/Vca_VTriggerTicket.cpp',
        '<(vision_src)/kernel/Vca_VTypeInfoHolder.cpp',
        '<(vision_src)/kernel/VkSocketAddress.cpp',
        '<(vision_src)/kernel/VPath.cpp',
        '<(vision_src)/kernel/VPeerData.cpp',
        '<(vision_src)/kernel/VPeerDataArray.cpp',
        '<(vision_src)/kernel/VTypeInfo.cpp',
      ],
    },

    ################
    ####  VcaMain
    ################
    {
      'target_name': 'VcaMain',
      'type': 'shared_library',
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'VCAMAIN_EXPORTS'
          ]
        }]
      ],
      'dependencies': [
        'V',
        'Vca'
      ],
      'sources': [
        '<(vision_src)/kernel/Vca_Main.cpp',
        '<(vision_src)/kernel/Vca_IAdmin.cpp',
        '<(vision_src)/kernel/Vca_IBuddy.cpp',
        '<(vision_src)/kernel/Vca_IDie.cpp',
        '<(vision_src)/kernel/Vca_IPauseResume.cpp',
        '<(vision_src)/kernel/Vca_IStop.cpp',
        '<(vision_src)/kernel/Vca_VApplication.cpp',
        '<(vision_src)/kernel/Vca_VApplicationContext.cpp',
        '<(vision_src)/kernel/Vca_VClientApplication.cpp',
        '<(vision_src)/kernel/Vca_VMainProgram.cpp',
        '<(vision_src)/kernel/Vca_VProgramContext.cpp',
        '<(vision_src)/kernel/Vca_VServerApplication.cpp',
      ]
    },

    ################
    ####  Vsa
    ################
    {
      'target_name': 'Vsa',
      'type': 'shared_library',
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'VSA_EXPORTS'
          ]
        }]
      ],
      'dependencies': [
        'V',
        'Vca'
      ],
      'sources': [
        '<(vision_src)/kernel/Vsa.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluation.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluationResult.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluator.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluator_Ex1.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorClient.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorControl.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPool.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPool_Ex1.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPool_Ex2.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPool_Ex3.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPool_Ex4.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPool_Ex5.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex1.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex2.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex3.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex4.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex5.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex6.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex7.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPoolSettings_Ex8.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPump.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorPumpSettings.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorSettings.cpp',
        '<(vision_src)/kernel/Vsa_IEvaluatorSourceControl.cpp',
        '<(vision_src)/kernel/Vsa_IPoolApplication.cpp',
        '<(vision_src)/kernel/Vsa_IPoolApplication_Ex1.cpp',
        '<(vision_src)/kernel/Vsa_IPoolApplication_Ex2.cpp',
        '<(vision_src)/kernel/Vsa_IPoolEvaluation.cpp',
        '<(vision_src)/kernel/Vsa_IPoolEvaluation_Ex1.cpp',
        '<(vision_src)/kernel/Vsa_IPoolWorker.cpp',
        '<(vision_src)/kernel/Vsa_ISubscription.cpp',
        '<(vision_src)/kernel/Vsa_IUpDownPublisher.cpp',
        '<(vision_src)/kernel/Vsa_IUpDownSubscriber.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluation.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluationResult.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluator.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluatorClient.cpp',
        '<(vision_src)/kernel/Vsa_VOdometer.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluatorPool.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluatorPump.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluatorPumpSource.cpp',
        '<(vision_src)/kernel/Vsa_VEvaluatorSource.cpp',
        '<(vision_src)/kernel/Vsa_VGenericEvaluation.cpp',
        '<(vision_src)/kernel/Vsa_VPathQuery.cpp',
        '<(vision_src)/kernel/Vsa_VPoolBroadcast.cpp',
        '<(vision_src)/kernel/Vsa_VPoolBroadcastEvaluation.cpp',
        '<(vision_src)/kernel/Vsa_VPoolEvaluation.cpp',
        '<(vision_src)/kernel/Vsa_VPoolWorker.cpp',
        '<(vision_src)/kernel/Vsa_VPoolWorkerGeneration.cpp',
        '<(vision_src)/kernel/Vsa_VRequestMaker.cpp',
        '<(vision_src)/kernel/Vsa_VsaDirectoryBuilder.cpp',
        '<(vision_src)/kernel/Vsa_VSmartEvaluatorSource.cpp',
      ]
    },

    ################
    ####  Vxa
    ################
    {
      'target_name': 'Vxa',
      'type': 'shared_library',
      'dependencies': [
        'V',
        'Vca'
      ],
      'conditions': [
        ['OS=="win"', {
          'defines': [
            'VXA_EXPORTS'
          ]
        }]
      ],
      'sources': [
        '<(vision_src)/kernel/Vxa.cpp',
        '<(vision_src)/kernel/Vxa_ICaller.cpp',
        '<(vision_src)/kernel/Vxa_ICaller2.cpp',
        '<(vision_src)/kernel/Vxa_ICollection.cpp',
        '<(vision_src)/kernel/Vxa_ISingleton.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskHolder.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskHolder1.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskHolder2.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskImplementation.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskImplementation2.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskImplementation3.cpp',
        '<(vision_src)/kernel/Vxa_IVSNFTaskImplementation3NC.cpp',
        '<(vision_src)/kernel/Vxa_VAny.cpp',
        '<(vision_src)/kernel/Vxa_VCLF.cpp',
        '<(vision_src)/kernel/Vxa_VCallAgent.cpp',
        '<(vision_src)/kernel/Vxa_VCallData.cpp',
        '<(vision_src)/kernel/Vxa_VCallHandle.cpp',
        '<(vision_src)/kernel/Vxa_VCallType1.cpp',
        '<(vision_src)/kernel/Vxa_VCallType1Exporter.cpp',
        '<(vision_src)/kernel/Vxa_VCallType2.cpp',
        '<(vision_src)/kernel/Vxa_VCallType2Exporter.cpp',
        '<(vision_src)/kernel/Vxa_VCardinalityHints.cpp',
        '<(vision_src)/kernel/Vxa_VClass.cpp',
        '<(vision_src)/kernel/Vxa_VCollectableCollection.cpp',
        '<(vision_src)/kernel/Vxa_VCollectableObject.cpp',
        '<(vision_src)/kernel/Vxa_VCollection.cpp',
        '<(vision_src)/kernel/Vxa_VCursor.cpp',
        '<(vision_src)/kernel/Vxa_VError.cpp',
        '<(vision_src)/kernel/Vxa_VExportable.cpp',
        '<(vision_src)/kernel/Vxa_VExportableDatum.cpp',
        '<(vision_src)/kernel/Vxa_VFiniteSet.cpp',
        '<(vision_src)/kernel/Vxa_VImportable.cpp',
        '<(vision_src)/kernel/Vxa_VInfiniteSet.cpp',
        '<(vision_src)/kernel/Vxa_VMap.cpp',
        '<(vision_src)/kernel/Vxa_VMapMaker.cpp',
        '<(vision_src)/kernel/Vxa_VMonotypeMap.cpp',
        '<(vision_src)/kernel/Vxa_VMonotypeMapMaker.cpp',
        '<(vision_src)/kernel/Vxa_VMonotypeMapMakerFor.cpp',
        '<(vision_src)/kernel/Vxa_VMethod.cpp',
        '<(vision_src)/kernel/Vxa_VPolytypeMap.cpp',
        '<(vision_src)/kernel/Vxa_VPolytypeMapMaker.cpp',
        '<(vision_src)/kernel/Vxa_VResultBuilder.cpp',
        '<(vision_src)/kernel/Vxa_VRolePlayer.cpp',
        '<(vision_src)/kernel/Vxa_VSerializer2DynArray.cpp',
        '<(vision_src)/kernel/Vxa_VSet.cpp',
        '<(vision_src)/kernel/Vxa_VTask.cpp',
        '<(vision_src)/kernel/Vxa_VTaskCursor.cpp',
        '<(vision_src)/kernel/Vxa_VTicketManager.cpp',
        '<(vision_src)/kernel/Vxa_VType.cpp',
      ]
    }
  ]
}
