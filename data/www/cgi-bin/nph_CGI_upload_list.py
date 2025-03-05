#!/usr/bin/env python3

from ResponseGenerator import ResponseGenerator
from upload_list.CGI import CGI
from generalChecks import general_checks
import sys
import os

# Need with env: filename / requestmethod / contentlength / querystring in UPPERCASE?
# rules:

# GET request upload list
# Start reading CONTENT_LENGTH from env -> needs for POST request
# Get all the other nessasery env -> needs CONTENT-TYPE as well
# Read body of the POST REQUEST from stdin
# Create path to where to store the image and checks extension
# Writes body to imagefile
# Creates response HTML and send back to stdout for server (Location Header)
# If something went wrong response HTML is errorpage

def main() -> int:
	if general_checks(False):
		return 1

	cgi_upload_list = CGI()
	try:
		cgi_upload_list.parsing()

		if cgi_upload_list.get_status_code() == 200:
			cgi_upload_list.generate_response()
			return
		else:
			ResponseGenerator("GET", cgi_upload_list.get_status_code())
			error_response = create_response()
			cgi_upload_list.write_to_stdout(error_response)

		return 0 if cgi_contact.get_status_code() == 200 else 1
	except Exception as e:
		print(f"Error CGI: {e}", file=sys.stderr)
	return 1


if __name__ == '__main__':
	sys.exit(main())