#!/usr/bin/env python3

# Need in env: request_method, contentlegnth, content-type
# Need with STDIN: body

# POST REQUEST CONTACT
# extract request target
# checks if env are there and reads from STDIN
# write info in format to file in uploads
# Creates response HTML and send back to stdout for server (Location Header)
# If something went wrong response HTML is errorpage

import sys
from ResponseGenerator import ResponseGenerator
from submit_contact_info.Parser import Parser
from submit_contact_info.CGI import CGI

def main() -> int:
	cgi_contact = CGI()

	try:
		cgi_contact.parsing()

		if cgi_contact.get_status_code() == 200:
			cgi_contact.process_request()

		cgi_contact.generate_response()

		return 0 if cgi_contact.get_status_code() == 200 else 1

	except Exception as e:
		print(f"Error in CGI: {e}", file=sys.stderr)

	try:
		cgi_contact.set_status_code(400)
		cgi_contact.generate_response()
	except Exception as inner_e:
		print(f"Error with generate response: {e}", file=sys.stderr)

	return 1


if __name__ == '__main__':
	sys.exit(main())