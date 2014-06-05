#!/bin/bash

# Never forget to build again!
pushd $CMSSW_BASE/src
scram b
popd

jobcount=''
doeff=true
dorate=true
while getopts ":rej:" opt; do
  case $opt in
    j)
      jobcount="--job-count=$OPTARG"
      ;;
    r)
      doeff=false
      ;;
    e)
      dorate=false
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

if $doeff; then
    rm /nfs_scratch/nsmith/egalg_eff* -r
    gsido rm /hdfs/store/user/nsmith/egalg_eff* -r
    farmoutAnalysisJobs \
        --input-dir=/store/mc/TTI2023Upg14D/SingleElectronFlatPt0p2To50/GEN-SIM-DIGI-RAW/PU140bx25_PH2_1K_FB_V3-v2/00000 \
        --input-files-per-job=1 $jobcount \
        egalg_eff_hists $CMSSW_BASE eff_hists_cfg.py
fi

if $dorate; then
    rm /nfs_scratch/nsmith/egalg_rate* -r
    gsido rm /hdfs/store/user/nsmith/egalg_rate* -r
    farmoutAnalysisJobs \
        --input-dir=/store/mc/TTI2023Upg14D/Neutrino_Pt2to20_gun/GEN-SIM-DIGI-RAW/PU140bx25_PH2_1K_FB_V3-v2/00000 \
        --input-files-per-job=1 $jobcount \
        egalg_rate_hists $CMSSW_BASE rate_hists_cfg.py
fi
