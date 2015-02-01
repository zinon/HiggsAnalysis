setupATLAS
localSetupDQ2Client --quiet
voms-proxy-init --voms atlas
localSetupPandaClient currentJedi --noAthenaCheck
rcSetup Base,2.0.23
rc find_packages
rc compile
