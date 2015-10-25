
timestamp() {
  date +"%s"
}

cd /wypok
./wykres data/
mv data/galk.png /ztmtoosm/www/
rm data/chart_data.ddd
rm data/chart_data.ustawienia
node tro.js
akttime=$(timestamp)
node get_best_voters.js data > "specjal/"$akttime".txt"
cd /
