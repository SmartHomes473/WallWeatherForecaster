#!/usr/bin/env python

# parameters:
ip = '141.212.20.135'

# ann arbor, brooklyn, austin, seattle and denver.
woeids = [2354842,2369943,2357536,2490383,2391279]
canadianRss = ['http://rss.theweathernetwork.com/weather/usmi0028','http://rss.theweathernetwork.com/weather/usmi1435','http://rss.theweathernetwork.com/weather/ustx0057','http://rss.theweathernetwork.com/weather/uswa0395','http://rss.theweathernetwork.com/weather/usco0105']

unit = 'f'

# imports
import sys,feedparser

data = []
f = open('weather.txt','w')
print sys.argv

for id in woeids :
    d = feedparser.parse('http://weather.yahooapis.com/forecastrss?w='+str(id)+'&u='+unit)
    cityName = d['feed']['subtitle'][19:]+' '
    high = d['entries'][0]['yweather_forecast']['high']
    low = d['entries'][0]['yweather_forecast']['low']
    humidity = d['feed']['yweather_atmosphere']['humidity']
    data += [[cityName,high,low,humidity,int(0)]]

i = 0
for url in canadianRss :
    d = feedparser.parse(url)
    data[i][4] = int(d['entries'][1]['summary_detail']['value'].partition('P.O.P.')[2].partition('%')[0])
    print data[i]
    i = i + 1 
for city in data:
    f.write(city[0]+','+city[1]+','+city[2]+','+city[3]+','+str(city[4])+'\n')

f.close()
