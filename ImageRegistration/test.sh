


cmake . -D USECUDA=1 -D USE_VTK=0
make -j 16

./bin/DRBUDDI_cuda -u ./test_data1/nodif.nii.gz -d ./test_data1/nodif_PA.nii.gz -s ./test_data1/struct.nii.gz --up_json ./test_data1/data.json --DRBUDDI_step 0