<?php
	
	try {
	
		// The Site to get Content From:
		define("TargetSite", "http://www.example.com");

		// The Full URL for our Request
		$RequestURL = TargetSite . "$_SERVER[REQUEST_URI]";
		
		// Make the Request
		$ResponseContent = file_get_contents($RequestURL);
		$ResponseMIMEType = GetMIMEType_FromResponseHeaders($http_response_header);
		$ResponseStatusCode = GetHTTPStatusCode_FromResponseHeaders($http_response_header);
		
		// Set the MIME Type of the fetched Content
		header("Content-type: " . $ResponseMIMEType);
		
		// Set the HTTP StatusCode of the fetched Content
		http_response_code($ResponseStatusCode);
				
		// Echo back the Response from the Remote Server
		//echo $ResponseContent;
		echo "Site coming soon...";
	
	} catch(Exception $E) {
		http_response_code(500);
		echo "Exception Thrown during Proxy Request: " . $E->getMessage();
	}
	
	function GetMIMEType_FromResponseHeaders($HTTPResponseHeaders) {
		
		// Default to text/plain if not resolved in the following loop
		$ResolvedMIMEType = "text/plain";
		
		// We are looking for a header like [Content-Type: application/x-javascript]
		foreach ($HTTPResponseHeaders as $Header) {
			
			// If the $Header starts with "Content-Type: "
			if (strtoupper(substr($Header, 0, 13)) === "CONTENT-TYPE:") {
				$ResolvedMIMEType = str_replace(" ", "", substr($Header, 13, (strlen($Header) - 13)));
			}
		}
		
		return $ResolvedMIMEType;
	}
	
	function GetHTTPStatusCode_FromResponseHeaders($HTTPResponseHeaders) {
		
		// Default to 200 if not resolved in the following loop
		$ResolvedStatusCode = 200;
		
		// We are looking for a header like [HTTP/1.1 200 OK]
		foreach ($HTTPResponseHeaders as $Header) {
			
			// If the $Header starts with "HTTP/1.1 ", and is longer than just "HTTP/1.1 "
			if ((strtoupper(substr($Header, 0, 9)) === "HTTP/1.1 ") and (strlen($Header) > 9)) {
				// Extract just the Integer from the $Header. (The substr() is to make sure we don't extract the 1.1 bit of the HTTP/1.1)
				$ResolvedStatusCode = filter_var(substr($Header, 9, (strlen($Header) - 9)), FILTER_SANITIZE_NUMBER_INT);
			}
		}
		
		return $ResolvedStatusCode;
	}
	
	// ---------- For Debugging ----------
	//echo "Proxy Page (v3)<br/>";
	//echo "Request URI: `" . $RequestURL . "`<br/><br/><br/><br/><br/>";
	//
	//echo "Response MIME Type: `" . $ResponseMIMEType . "`\r\n";
	//echo "Response STAT Code: `" . $ResponseStatusCode . "`";
?>	