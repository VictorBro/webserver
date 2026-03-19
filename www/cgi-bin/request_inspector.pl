#!/usr/bin/env perl
use strict;
use warnings;

# ------------------------------------------------------------
# Read POST body if present
# ------------------------------------------------------------
my $body = '';
if ( ( $ENV{REQUEST_METHOD} // '' ) eq 'POST' ) {
    my $len = int( $ENV{CONTENT_LENGTH} // 0 );
    read( STDIN, $body, $len ) if $len > 0;
}

# ------------------------------------------------------------
# HTML-escape helper
# ------------------------------------------------------------
sub esc {
    my $s = shift // '';
    $s =~ s/&/&amp;/g;
    $s =~ s/</&lt;/g;
    $s =~ s/>/&gt;/g;
    $s =~ s/"/&quot;/g;
    return $s;
}

# ------------------------------------------------------------
# Collect and sort CGI environment variables
# ------------------------------------------------------------
my @cgi_keys = sort grep {
    /^(?:AUTH_TYPE|CONTENT_|GATEWAY_INTERFACE|HTTP_|PATH_|
         QUERY_STRING|REMOTE_|REQUEST_|SCRIPT_|SERVER_)/x
} keys %ENV;

my $rows = '';
for my $key (@cgi_keys) {
    $rows .= sprintf(
        "      <tr><td class=\"key\">%s</td><td class=\"val\">%s</td></tr>\n",
        esc($key), esc( $ENV{$key} )
    );
}
$rows ||= "      <tr><td colspan=\"2\" class=\"empty\">No CGI variables found</td></tr>\n";

my $body_section = '';
if ( $body ne '' ) {
    $body_section = sprintf(
        "<h2>Request Body</h2>\n    <pre class=\"body\">%s</pre>",
        esc($body)
    );
}

# ------------------------------------------------------------
# Output
# ------------------------------------------------------------
print "Content-Type: text/html; charset=UTF-8\r\n\r\n";
print <<"HTML";
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Request Inspector</title>
  <style>
    body { font-family: monospace; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 2rem; }
    h1   { color: #cba6f7; margin-bottom: 0.25rem; }
    h2   { color: #89b4fa; margin-top: 1.5rem; }
    p.sub { color: #6c7086; margin-top: 0; margin-bottom: 1rem; font-size: 0.85rem; }
    table { border-collapse: collapse; width: 100%; max-width: 860px; }
    td   { padding: 0.35rem 0.6rem; border-bottom: 1px solid #313244; vertical-align: top; }
    td.key { color: #89dceb; width: 34%; white-space: nowrap; }
    td.val { color: #a6e3a1; word-break: break-all; }
    td.empty { color: #6c7086; font-style: italic; }
    pre.body { background: #181825; padding: 1rem; border-radius: 6px;
               white-space: pre-wrap; word-break: break-all; color: #fab387;
               max-width: 860px; }
    a    { color: #89b4fa; }
  </style>
</head>
<body>
  <h1>Request Inspector</h1>
  <p class="sub">CGI environment variables passed by the server for this request</p>
  <h2>CGI Environment</h2>
  <table>
$rows  </table>
  $body_section
  <p style="margin-top:2rem"><a href="../index.html">&larr; Home</a></p>
</body>
</html>
HTML
