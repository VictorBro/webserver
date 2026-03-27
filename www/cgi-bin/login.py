#!/usr/bin/env python3
import cgi
import cgitb
import os
import sys
import random
import string
import http.cookies
import urllib.parse

env_debug = os.environ.get("DEBUG", "")
if env_debug == "1":
	cgitb.enable()
	sys.stderr.write("===== ENVIRONMENT VARIABLES =====\n")
	for key, value in os.environ.items():
		sys.stderr.write(f"{key}: {value}\n")
	sys.stderr.write("================================\n")

def generate_session_id(length=32):
	chars = string.ascii_letters + string.digits
	return ''.join(random.choice(chars) for _ in range(length))

def get_current_session():
	cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
	if "session_id" in cookie:
		session_id = cookie["session_id"].value
		session_file = f"/tmp/sess_{session_id}"
		if os.path.exists(session_file):
			try:
				with open(session_file, "r") as f:
					username = f.read().strip()
					return username, session_id, None
			except Exception as e:
				return None, session_id, f"Error reading session file: {str(e)}"
		else:
			return None, session_id, "Session expired or not found"
	return None, None, None

query_string = os.environ.get("QUERY_STRING", "")
params = urllib.parse.parse_qs(query_string)
logout = "logout" in params

username, session_id, session_error = get_current_session()

if logout and session_id:
	session_file = f"/tmp/sess_{session_id}"
	try:
		if os.path.exists(session_file):
			os.remove(session_file)
	except:
		pass

	sys.stdout.write("Content-type: text/html\r\n")
	sys.stdout.write("Set-Cookie: session_id=deleted; Path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n")
	sys.stdout.write("\r\n")
	sys.stdout.write("""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Logged Out</title>
	<link rel="stylesheet" href="/style/styles.css">
	<meta http-equiv="refresh" content="2;url=/pages/login.html">
</head>
<body>
	<nav><ul><li><a href="/index.html">Home</a></li></ul></nav>
	<main>
		<div class="wrapper">
			<h1 class="subtitle">Logged Out</h1>
			<p>You have been logged out. Redirecting to login page...</p>
			<a href="/pages/login.html" class="button">Back to Login</a>
		</div>
	</main>
</body>
</html>
""")
	sys.exit(0)

if session_id and session_error:
	sys.stdout.write("Content-type: text/html\r\n")
	sys.stdout.write("Set-Cookie: session_id=deleted; Path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n")
	sys.stdout.write("\r\n")
	sys.stdout.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Session Error</title>
	<link rel="stylesheet" href="/style/styles.css">
</head>
<body>
	<nav><ul><li><a href="/index.html">Home</a></li></ul></nav>
	<main>
		<div class="wrapper">
			<h1 class="subtitle">Session Error</h1>
			<div class="line"></div>
			<p>Your session has expired. Please log in again.</p>
			<a href="/pages/login.html" class="button">Log In</a>
		</div>
	</main>
</body>
</html>
""")
	sys.exit(0)

if username and session_id and not logout:
	sys.stdout.write("Content-type: text/html\r\n")
	sys.stdout.write("\r\n")
	sys.stdout.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Welcome {username}</title>
	<link rel="stylesheet" href="/style/styles.css">
</head>
<body>
	<nav><ul><li><a href="/index.html">Home</a></li></ul></nav>
	<main>
		<div class="wrapper">
			<h1 class="subtitle">Hello, {username}!</h1>
			<div class="line"></div>
			<p>You are currently logged in.</p>
			<p><strong>Session ID:</strong> {session_id}</p>
			<a href="/cgi-bin/login.py?logout=1" class="button">Logout</a>
			<a href="/" class="button">Home</a>
		</div>
	</main>
</body>
</html>
""")
	sys.exit(0)

try:
	form = cgi.FieldStorage()
	username = form.getvalue("username")
except Exception as e:
	sys.stdout.write("Content-type: text/html\r\n\r\n")
	sys.stdout.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<title>Error</title>
	<link rel="stylesheet" href="/style/styles.css">
</head>
<body>
	<main>
		<div class="wrapper">
			<h1 class="subtitle">Error</h1>
			<p>An error occurred: {str(e)}</p>
			<a href="/pages/login.html" class="button">Back to Login</a>
		</div>
	</main>
</body>
</html>
""")
	sys.exit(1)

sys.stdout.write("Content-type: text/html\r\n")

if username:
	try:
		session_id = generate_session_id()
		session_file = f"/tmp/sess_{session_id}"
		with open(session_file, "w") as f:
			f.write(username)

		sys.stdout.write(f"Set-Cookie: session_id={session_id}; Path=/; Max-Age=3600\r\n")
		sys.stdout.write("\r\n")
		sys.stdout.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Welcome {username}</title>
	<link rel="stylesheet" href="/style/styles.css">
</head>
<body>
	<nav><ul><li><a href="/index.html">Home</a></li></ul></nav>
	<main>
		<div class="wrapper">
			<h1 class="subtitle">Welcome, {username}!</h1>
			<div class="line"></div>
			<p>You have successfully logged in.</p>
			<p><strong>Session ID:</strong> {session_id}</p>
			<p><strong>Cookie expiration:</strong> 1 hour</p>
			<a href="/cgi-bin/login.py?logout=1" class="button">Logout</a>
			<a href="/" class="button">Home</a>
		</div>
	</main>
</body>
</html>
""")
	except Exception as e:
		sys.stdout.write("\r\n")
		sys.stdout.write(f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Login Error</title>
	<link rel="stylesheet" href="/style/styles.css">
</head>
<body>
	<main>
		<div class="wrapper">
			<h1 class="subtitle">Login Error</h1>
			<p>An error occurred: {str(e)}</p>
			<a href="/pages/login.html" class="button">Try Again</a>
		</div>
	</main>
</body>
</html>
""")
else:
	sys.stdout.write("Location: /pages/login.html\r\n")
	sys.stdout.write("\r\n")
