echo installing 
echo installing cmake
sudo apt install cmake -y
echo 
#stb
echo installing STB
git clone https://github.com/nothings/stb.git
echo STB install done
# GLM
echo installing GLM
git clone https://github.com/g-truc/glm.git
echo GLM Install done
# Bullet installation:
# See http://www.scratchpadgames.net/forums/showthread.php?tid=24
sudo apt install libbullet-dev -y
echo Bullet installed.
#XrandR
echo installing XrandR for Resolution changes
sudo apt install libxrandr-dev -y
echo  Xrandr installed
# RakNet installation:
echo RakNet install cannot be finished here, There are some edits needed, you must use an editor outside this console.
#git clone https://github.com/facebookarchive/RakNet.git /home/pi/RakNet
#cd /home/pi/RakNet
#cmake .
#echo Removing old RakNet installation if it exists.
#sudo rm -rf /usr/local/include/raknet
#echo You must make edits to ReplicaManager3.cpp at line 141, change -1 to nullptr
#read -s -p "Make the edits, using an editor, then Return here and press enter to continue"
#echo ... 
#echo Continuing with setup
#make
#make install
#sudo mv Lib/LibStatic/libRakNetLibStatic.a /usr/local/lib
#sudo mv include/raknet /usr/local/include/
#echo Cleaning up...
#rm -rf /home/pi/RakNet
echo RakNet installed.
echo installing libopenAL and sound libs
sudo apt install libopenal-dev -y
sudo apt install pulseaudio -y
sudo apt install libalut-dev -y
sudo apt install libogg-dev -y
sudo apt install libvorbis-dev -y
sudo apt install vorbis-tools -y
echo we have installed 
echo get latest mesa libs 
sudo apt install libgles2-mesa-dev -y
echo libmesa is Done
# Install FreeType
echo Installing freetype...
sudo apt install libfreetype6 libfreetype6-dev -y
wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.4.tar.gz
cd freetype-2.10.4
make
sudo make install
echo FreeType installed
echo installing Assimp
cd..
sudo apt install libassimp-dev
echo Assimp installed 
echo installing VLC
sudo apt install libvlc-dev
echo VLC Instllaed
echo enhancing GPU debug output not all will install
sudo apt install libglx-mesa0-dbgsym libglapi-mesa-dbgsym libgl1-mesa-dri-dbgsym
echo Done!
echo
echo Scroll up to make sure there were no errors, warnings are ok
echo Take note of the errors and check the web/support sites for fixes
read -s -p "Press Enter when you're ready!"
echo ...