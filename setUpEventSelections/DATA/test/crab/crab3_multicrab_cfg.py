from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.transferOutputs = True
config.General.transferLogs = True
config.JobType.allowUndistributedCMSSW = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'ConfFile_cfg.py'
config.Data.inputDataset = '/HIMinimumBias1/XeXeRun2017-PromptReco-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.lumiMask = 'Cert_304899-304906_XeXe_PromptReco_Collisions17_JSON.txt'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2
config.Data.outLFNDirBase = '/store/user/tuos/XeXe/Flow/TestCentralityRun/v01Test_Cent_oct25'
config.Data.publication = False
config.Data.outputDatasetTag = 'xexe2017_flowcorr_HIMinimumBias1_v01Test_Cent_oct25'

config.Site.storageSite = 'T2_US_Vanderbilt'

if __name__ == '__main__':
   from CRABAPI.RawCommand import crabCommand
   from CRABClient.ClientExceptions import ClientException
   from httplib import HTTPException

   config.General.workArea = 'xexe2017_flowcorr_HIMinimumBias1_v01Test_Cent_oct25'

   def submit(config):
      try:
           crabCommand('submit', config = config)
      except HTTPException as hte:
           print "Failed submitting task: %s" % (hte.headers)
      except ClientException as cle:
          print "Failed submitting task: %s" % (cle)
   
   for num in range(0,20):

       print "Submitting Data Set %d " % (num+1)

       RequestName = 'xexe2017_flowcorr_HIMinimumBias%d_v01Test_Cent_oct25' % (num+1)
       DataSetName = '/HIMinimumBias%d/XeXeRun2017-PromptReco-v1/AOD' % (num+1)
           
       config.General.requestName = RequestName
       config.Data.inputDataset = DataSetName
       submit(config)

# python crab3_multicrab_cfg.py to execute 
# ./multicrab -c status -w crab_projects/ to check status 
