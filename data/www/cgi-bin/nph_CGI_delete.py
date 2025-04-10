#!/usr/bin/env python3

from ResponseGenerator import ResponseGenerator
from delete.DeleteFile import DeleteFile
from delete.CGI import CGI
from generalChecks import general_checks

import sys
import os

# Need with env: filename / requestmethod / contentlength / querystring in UPPERCASE?
# rules:

# DELETE request
# Get all the env
# Check if the file is there
# Delete file
# Creates response HTML and send back to stdout for server (Location Header)
# If something went wrong response HTML is errorpage
# USE: curl -X DELETE localhost:8080/text.txt

def main() -> int:
	if general_checks(False):
		return 1

	cgi_delete = CGI()
	try:
		cgi_delete.parsing()
		if cgi_delete.get_status_code() == 200:
			cgi_delete.process_request()
		cgi_delete.generate_response()
	except Exception as e:
		print(f"Error CGI: {e}", file=sys.stderr)
		try:
			cgi_delete.set_status_code(400)
			cgi_delete.generate_response()
		except Exception as e:
			print(f"Error with generate_response: {e}", file=sys.stderr)
		return 400
	# if the delete is good or not found it has a good return msg
	if cgi_delete.get_status_code() == 200 or cgi_delete.get_status_code() == 404:
		return 0
	else:
		return 1

if __name__ == '__main__':
	sys.exit(main())