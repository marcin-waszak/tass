import gmplot
import webbrowser
import os
import csv

color_map = { 1: 'blue', 2: 'green', 3: 'red', 4: 'cyan', 5: 'magenta', 6: 'yellow',
            7: 'black', 8: 'maroon', 9: 'pink', 10: 'plum', 11: 'aliceblue', 12: 'fuchsia',
            13: 'gainsboro', 14: 'indianred', 15: 'khaki', 16: 'ivory', 17: 'yellowgreen', 18: 'tan',
            19: 'tomato',20: 'orangered', 21: 'sandybrown', 22: 'salmon', 23: 'paleturquoise', 24: 'olive',
            25: 'lavender', 26: 'springgreen', 27: 'silver', 28: 'royalblue', 29: 'hotpink', 30: 'honeydew',
            31: 'firebrick', 32: 'darkviolet', 33: 'darkorchid', 34: 'darkgreen', 35: 'burlywood', 37: 'goldenrod',
            36: 'lemonchiffon', 38: 'lime', 39: 'sienna', 40: 'skyblue', 41: 'papayawhip', 42: 'mediumblue'}


def create_map(filename, mapname):
	with open(filename) as f:
		reader = csv.reader(f)	
	        l = []
	        a = []
	        t = []
	        for line in f:
	        	items = line.split(",")
       			l.append(float(items[10]))
			a.append(float(items[11]))
            		t.append(items[1])

		gmap = gmplot.GoogleMapPlotter(l[0], a[0], 3)
        	for i in range(len(l)):
			gmap.marker(l[i], a[i], color=color_map[i + 1], title=t[i])
        	gmap.draw(mapname)
		
		webbrowser.open('file://' + os.path.realpath(mapname))


if __name__ == "__main__":
	create_map("result_data/result_tourism.csv", "maps/result_tourism_map.html")
	create_map("result_data/result_deg.csv", "maps/result_deg_map.html")
	create_map("result_data/result_arrivals_to_deg.csv", "maps/result_arrivals_to_deg_map.html")
	create_map("result_data/result_long_to_short.csv", "maps/result_long_to_short_map.html")
	create_map("result_data/result_short_to_long.csv", "maps/result_short_to_long_map.html")
	create_map("result_data/result_long_to_deg.csv", "maps/result_long_to_deg_map.html")
	create_map("result_data/result_long_to_pop.csv", "maps/result_long_to_pop_map.html")
