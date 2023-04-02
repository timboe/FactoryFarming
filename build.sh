rm -r FactoryFarming.pdx
rm -r FactoryFarmingTechDemo.pdx

sed 's|FactoryFarming.pdx|FactoryFarmingTechDemo.pdx|g' Makefile  --in-place=.bak
sed 's|//#define DEMO|#define DEMO|g' src/constants.h --in-place=.bak
sed 's|imagePath=titles|imagePath=demo|g' Source/pdxinfo --in-place=.bak
sed 's|Factory Farming|Factory Farming Tech Demo|g' Source/pdxinfo --in-place=.bak
sed 's|uk.co.tim-martin.factoryfarming|uk.co.tim-martin.factoryfarmingtechdemo|g' Source/pdxinfo --in-place=.bak

rm *bak
rm Source/*bak
rm src/*bak

make

sed 's|FactoryFarmingTechDemo.pdx|FactoryFarming.pdx|g' Makefile --in-place=.bak
sed 's|#define DEMO|//#define DEMO|g' src/constants.h --in-place=.bak
sed 's|imagePath=demo|imagePath=titles|g' Source/pdxinfo --in-place=.bak
sed 's|Factory Farming Tech Demo|Factory Farming|g' Source/pdxinfo --in-place=.bak
sed 's|uk.co.tim-martin.factoryfarmingtechdemo|uk.co.tim-martin.factoryfarming|g' Source/pdxinfo --in-place=.bak

rm *bak
rm Source/*bak
rm src/*bak

make

rm -r FactoryFarming.pdx/demo/
rm -r FactoryFarmingTechDemo.pdx/titles/
rm FactoryFarming.pdx/player_2.json
rm FactoryFarming.pdx/world_2_1.json


rm FactoryFarmingTechDemo_v1.3.pdx.zip
rm FactoryFarming_v1.3.pdx.zip
rm FactoryFarming_NoMusic_v1.3.pdx.zip

zip -r -q FactoryFarmingTechDemo_v1.3.pdx.zip FactoryFarmingTechDemo.pdx

zip -r -q FactoryFarming_v1.3.pdx.zip FactoryFarming.pdx

mv FactoryFarming.pdx/music .

zip -r -q FactoryFarming_NoMusic_v1.3.pdx.zip FactoryFarming.pdx

mv music FactoryFarming.pdx

