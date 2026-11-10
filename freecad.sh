# This is going to check if the git rope is present and then clone it if its not..  
if git ls-remote "https://github.com/FreeCAD/FreeCAD-library" -q; then
    echo "Cloning the git repo for the FreeCAD data this is quite large ~."
    git clone https://github.com/FreeCAD/FreeCAD-library
fi
cp freeCAD_image_builder.py FreeCAD-library/ripper.py
cd FreeCAD-library
echo "pip installing the correct packages."
pip install numpy trimesh
echo "Starting reaster to 256x256 pixels for ten layers for each .stl, this is going to take a while (go make some coffee) (if you want to change this you should mod the hyperparams in the directory datasets/freecad_image_builder.py )"
python ripper.py
mv outputs ../outputs
rm -r layers ripper.py list.txt
cd ..
