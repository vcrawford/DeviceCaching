num_days=15
start_time=8
contact_radius=20
startn=20
endn=500
incn=5
speed=1

for ((i=$startn;i<=$endn;i=i + $incn));
do

   time=$(date +%s)
   location_output_file="about_location_"$time".xml"
   location_data_file="location_"$time".txt"
   contact_graph_file="contact_graph_"$time".txt"

   echo "<LocationsAbout id=\""$time"\">" >> $location_output_file
   echo " <days>"$num_days"</days>" >> $location_output_file
   echo " <hour>"$start_time"</hour>" >> $location_output_file
   echo " <radius>"$contact_radius"</radius>" >> $location_output_file
   echo " <n>"$i"</n>" >> $location_output_file
   echo " <speed>"$speed"</speed>" >> $location_output_file
   echo "</LocationsAbout>" >> $location_output_file

   ./Slaw/RunSlaw $i $num_days $start_time $speed $location_data_file
   ./Slaw/ContactGraph $i $location_data_file $contact_radius $contact_graph_file
done


