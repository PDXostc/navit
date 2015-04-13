1. How to copy additional maps

You can download OpenStreet maps in bin format directly from http://maps5.navit-project.org/
Next copy bin file to /usr/share/navit/maps/ on the target.

2. How to enable external GPS receiver

Edit file /usr/share/navit/navit.xml:

Edit line:
<vehicle name="Local GPS" profilename="car" enabled="no" active="1" source="gpsd://localhost" gpsd_query="w+xj">
Change enabled to "yes"

Edit line:
<vehicle name="Demo" profilename="car" enabled="yes" active="yes" source="demo://"/>
Change enabled to "no"

3. How to simulate gps data from file

Edit file /usr/share/navit/navit.xml:

- Find lines with vehicle name="Local GPS" and vehicle name="Demo" and set enabled="no" for both of them

- Add line
<vehicle name="Simulation GPS" profilename="car" enabled="yes" active="yes" source="file:/path/to/file.log"/>

where /path/to/file.log is a path to file with GPS data

For more info look at:
http://wiki.navit-project.org/index.php/Configuration#Vehicle_Options
