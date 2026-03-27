#!/usr/bin/env python3
import sys
import json
import urllib.request
import html as html_mod

WMO_CODES = {
    0: "Clear sky", 1: "Mainly clear", 2: "Partly cloudy", 3: "Overcast",
    45: "Foggy", 48: "Icy fog",
    51: "Light drizzle", 53: "Drizzle", 55: "Heavy drizzle",
    61: "Light rain", 63: "Rain", 65: "Heavy rain",
    71: "Light snow", 73: "Snow", 75: "Heavy snow", 77: "Snow grains",
    80: "Light showers", 81: "Showers", 82: "Heavy showers",
    85: "Snow showers", 86: "Heavy snow showers",
    95: "Thunderstorm", 96: "Thunderstorm with hail", 99: "Heavy thunderstorm with hail",
}

def fetch(url, timeout=5):
    req = urllib.request.Request(url, headers={"User-Agent": "python-weather-cgi/1.0"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        return json.loads(r.read().decode("utf-8", "replace"))

city = "Unknown"
lat, lon = 46.5, 6.6  # fallback: Lausanne area

try:
    info = fetch("https://ipinfo.io/json")
    city = info.get("city", city)
    loc = info.get("loc", "")
    if loc and "," in loc:
        lat, lon = loc.split(",")
except Exception:
    pass

temp = wind = condition = "N/A"
try:
    url = (
        f"https://api.open-meteo.com/v1/forecast"
        f"?latitude={lat}&longitude={lon}&current_weather=true"
    )
    data = fetch(url)
    cw = data["current_weather"]
    temp = f"{cw['temperature']} \u00b0C"
    wind = f"{cw['windspeed']} km/h"
    condition = WMO_CODES.get(cw["weathercode"], f"Code {cw['weathercode']}")
except Exception as e:
    condition = f"Error: {e}"

sys.stdout.write("Content-Type: text/html; charset=UTF-8\r\n\r\n")
sys.stdout.write(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Weather</title>
    <link rel="icon" href="/favicon.ico">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Eczar:wght@400;700;800&display=swap">
    <link rel="stylesheet" href="/style/styles.css">
</head>
<body>
    <nav><ul><li><a href="/">Home</a></li></ul></nav>
    <main>
        <h2 class="subtitle">Current Weather</h2>
        <div class="line"></div>
        <div class="wrapper" style="min-width: 280px;">
            <h3 class="subtitle">{html_mod.escape(city)}</h3>
            <p style="font-size: 2.5rem; font-weight: 800; margin: 0.5rem 0;">{html_mod.escape(temp)}</p>
            <p style="font-size: 1.1rem; color: var(--muted);">{html_mod.escape(condition)}</p>
            <p class="muted" style="margin-top: 0.5rem;">Wind: {html_mod.escape(wind)}</p>
        </div>
        <p class="muted" style="margin-top: 1.5rem;">Data from <a href="https://open-meteo.com">open-meteo.com</a></p>
        <a href="/" class="button" style="margin-top: 1rem;">Home</a>
    </main>
</body>
</html>
""")
