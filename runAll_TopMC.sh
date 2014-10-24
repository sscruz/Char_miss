#!/bin/bash

echo "Setting up environment..."
source /nfs/fanae/root_releases/root.5.34.13.slc5/bin/thisroot.sh
source /nfs/fanae/PAF_releases/PAF_devel/PAF_setup.sh
cd /nfs/fanae/user/sscruz/ChargeMiss/

##SEQ PWD=`pwd`
##SEQ mkdir -p jobs
##SEQ 
##SEQ echo "Sent multiple sequential jobs..."
##SEQ SAMPLES="TTGJets TTWJets TTZJets TTWWJets WWGJets WWWJets WWZJets WZZJets ZZZJets DYJets_Madgraph Wbb_Madgraph TWDilep TbarWDilep WgammaToLNuG ZgammaToLLG WWTo2L2Nu_Madgraph WZ ZZ" 
##SEQ 
##SEQ echo "Sending Dibosons, Single Top, Single Boson, TTV and VVV ..."
##SEQ for SAMPLE in $SAMPLES
##SEQ do
##SEQ     echo "Preparing to launch $sample... "
##SEQ     JOBFILE="jobs/$SAMPLE.sh"
##SEQ     cat > $JOBFILE <<EOF
##SEQ #!/bin/bash
##SEQ #PBS -e $PWD/jobs/$SAMPLE.err
##SEQ #PBS -o $PWD/jobs/$SAMPLE.log
##SEQ echo "Setting the environment for ROOT..."
##SEQ source /nfs/fanae/root_releases/root.5.34.13.slc5/bin/thisroot.sh
##SEQ echo "+ ROOTSYS = $ROOTSYS"
##SEQ 
##SEQ echo "Setting the environment for PAF..."
##SEQ source /nfs/fanae/PAF_releases/PAF_devel/PAF_setup.sh 
##SEQ echo "+ PAFPATH = $PAFPATH"
##SEQ 
##SEQ echo "Moving to $PWD..."
##SEQ cd $PWD
##SEQ echo -n "+ "
##SEQ pwd
##SEQ 
##SEQ root -l -b -q "RunTree_ReReco.C(\"${SAMPLE}\", 1, true, false)";
##SEQ 
##SEQ EOF
##SEQ     chmod u+x $JOBFILE
##SEQ     echo "Sending $SAMPLE"
##SEQ     qsub $JOBFILE
##SEQ     sleep 10
##SEQ done

echo "Now start PoD..." 
source /opt/PoD/PoD_env.sh

resetpaf -a
root -l -b -q RunTree_ReReco.C\(\"DY_CSA14\",60\)
root -l -b -q RunTree_ReReco.C\(\"ElGun_CSA14\",7\)

root -l -b -q RunTree_ReReco.C\(\"DYJets_Madgraph\",15\)
root -l -b -q RunTree_ReReco.C\(\"ZJets_Madgraph\",60\)