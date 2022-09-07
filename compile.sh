export CURRENT_DIR=$(pwd) 


cd $LOCAL_DIR/build #LOCALDIR is set by the setup.sh script!
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make install

cd $CURRENT_DIR