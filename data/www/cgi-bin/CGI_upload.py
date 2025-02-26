#!/usr/bin/env python3

from ResponseGenerator import ResponseGenerator
from upload.UploadFile import UploadFile
from upload.CGI import CGI
import sys

# Need with env: filename / requestmethod / contentlength / querystring in UPPERCASE?
# rules:

# POST request
# Start reading CONTENT_LENGTH from env -> needs for POST request
# Get all the other nessasery env -> needs CONTENT-TYPE as well
# Read body of the POST REQUEST from stdin
# Create path to where to store the image and checks extension
# Writes body to imagefile
# Creates response HTML and send back to stdout for server (Location Header)
# If something went wrong response HTML is errorpage

def main() -> int:
	cgi_upload = CGI()
	try:
		cgi_upload.parsing()
		if cgi_upload.get_status_code() == 200:
			cgi_upload.process_request()
		cgi_upload.generate_response()
	except Exception as e:
		print(f"Error CGI: {e}", file=sys.stderr)
		try:
			cgi_upload.set_status_code(400)
			cgi_upload.generate_response()
		except Exception as e:
			print(f"Error with generate_response: {e}", file=sys.stderr)
		return 400

	if cgi_upload.get_status_code() == 200:
		return 0
	return cgi_upload.get_status_code()

if __name__ == '__main__':
	sys.exit(main())