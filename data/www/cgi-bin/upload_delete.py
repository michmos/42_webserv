#!/usr/bin/env python3

from upload_delete.response_generator import ResponseGenerator
from upload_delete.file_operations import DeleteFile, UploadFile
from upload_delete.cgi_handler import CGI
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

# DELETE request
# Get all the env
# Check if the file is there
# Delete file
# Creates response HTML and send back to stdout for server (Location Header)
# If something went wrong response HTML is errorpage
# USE: curl -X DELETE localhost:8080/text.txt

def main() -> int:
	my_cgi = CGI()
	try:
		my_cgi.parsing()
		if my_cgi.get_status_code() == 200:
			my_cgi.process_request()
		my_cgi.generate_response()
	except Exception as e:
		sys.stderr.write(f"Error: {e}\n")
		try:
			my_cgi.set_status_code(400)
			my_cgi.generate_response()
		except Exception as e:
			sys.stderr.write(f"Error: {e}\n")
		return (400)
	return my_cgi.get_status_code()

if __name__ == '__main__':
	sys.exit(main())