Some instructions how to setup maps in navit:
---------------------------------------------

1. In navit_qt5 version v1.1.1 the example map with Portland, Oregon 
is present in rpm package, so to have this map on the target just rebuild the package
and reinstall this on the target. The map will be installed during rpm package installation.

2. Before the first launch try do on the target:

rm -f ~/.navit/center.txt

3. After the first lunch you should see your map centered at 4531 N 12240 W

4. If you want to put additional maps, copy them to /usr/share/navit/maps 
and remember to change map center point in /usr/share/navit/navit.xml
See for details: http://wiki.navit-project.org/index.php/Configuration


Some instructions how to use 'demo mode':
-----------------------------------------------------------------
In navit_qt5 version v1.1.1 after rpm package installation default working mode is demo mode.
You can use this without GPS data source. Just first click on point of the map where you want 
to have initial position of your demo car, next go to Menu->Actions->Clicked Position->Set as position.
Return to the map, click the point you have as a destination, next go to Menu->Actions->Clicked Position->Set as destination.
The route should appear on the map and the demo car should start the route.


Some instructions how to switch from 'demo mode' to 'Local GPS' :
-----------------------------------------------------------------
You should change some settings in navit.xml file:

Find line:
<vehicle name="Demo" profilename="car" enabled="yes" active="yes" source="demo://"/>
and set enabled="no"

Find line:
<vehicle name="Local GPS" profilename="car" enabled="no" active="1" source="gpsd://localhost" gpsd_query="w+xj">
and set enabled="yes".


You can do this before build by editing navit.xml in source code or
after instalaltion on the target:

1. Before build:
Edit Navigation/navit_qt5/custom/navit.xml
Next rebuild rpm package and reinstall this on the target.

2. After build and installation:
Edit on the target /usr/share/navit/navit.xml
And restart the navit application.



