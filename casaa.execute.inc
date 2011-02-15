<?php


#################################################
# Core Output Functions                         #
#################################################

/**
 * Implementation of hook_init()
* @global $exception_found flag to indicate that an exception was found.
* @global $script_written flag to prevent script from being included more than once per page load.
*
* @param $plugins a list of currently installed plugins
* @param $script_values the values from the exceptions settings for each plugin (if it has any)
*
* On page load check each plugin to see if it has any exception values that override any tax term mappings
* If so, set the exception flag to stop processing for that plugin and execute the plugin's output hook
* function.
*
*/
function casaa_init() {
	global $exception_found;
	global $script_written;
	
	if (drupal_is_front_page()) {
		$current_path = '[[front]';
	}
	else {
		$current_path = drupal_get_path_alias($_GET['q']);
   	if ($current_path != $_GET['q']) {
   		$current_path = $_GET['q'];
   	}
	}
	$plugins = _casaa_load_plugins();
	if (module_exists('domain')) {
		$domain = domain_get_domain();
	}
	else {
		$domain['domain_id'] = 0;
	}

	foreach ($plugins as $plugin => $values) {
		$script_values = casaa_path_match($current_path, $plugin, $domain['domain_id']);
		if ($script_values) {
			$hook_output = $plugin . '_output';
			if (function_exists($hook_output)) {
				$hook_output($script_values, unserialize($values['globals']));
				$script_written[$plugin] = TRUE;
			}
		}
	}
}


/**
 * For paths we do a two part check. First we query for an exact match of the URI
 * if we don't get a match we then pull a list of paths that contain the 
 * wildcard '*' symbol and compare them to the URI.
 **/
function casaa_path_match($path, $token, $domain) {

	//test for 404. we also want to avoid overriding any actual mapped paths.
	if ($path !== "[[front]") {	
		$not_found_test = drupal_get_normal_path(substr($path, 1));
		$path_query = db_query("SELECT casaa_path FROM {casaa_paths}");
		while ($test = db_fetch_object($path_query)) {
		  if (strstr(substr($path, 1), str_replace('*', '',str_replace('/*', '', $test->casaa_path)))) {
		    $tmp_path=$path;
		    break;
		  }
		  else if ($path == '/' . $not_found_test && !strstr(substr($path, 1), str_replace('*', '',str_replace('/*', '', $test->casaa_path)))) {
			 	//added an extra char at the beginning because of the substr in the query below.
		   	$tmp_path = "[[404]";
		  }
		}
		if ($tmp_path !== $path) {
		  $path = $tmp_path;
		}
	}
		
	//check for any paths that are an exact match
	$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_paths} p ON c.aid = p.aid WHERE c.casaa_domain=%d AND c.casaa_type='%s' AND (p.casaa_path='%s' OR p.casaa_path='%s' )",$domain, $token, substr($path, 1), substr($path, 1) . '/*');
	$result = db_fetch_object($query);
	//$wild_enabled = variable_get('casaa_wildcard', 0);
	$wild_enabled = TRUE;
	if ($result->casaa_value) {
		return $result->casaa_value;
	}
	else if ($wild_enabled) {
		$query = db_query("SELECT casaa_path FROM	{casaa_paths} WHERE casaa_path LIKE '%*'");
		while ($result = db_fetch_object($query)) {
			$beginning_path = str_replace('*', '', $result->casaa_path);
			if (strstr(substr($path, 1), $beginning_path)) {
				$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_paths} p ON c.aid = p.aid WHERE c.casaa_domain=%d AND p.casaa_path='%s' AND c.casaa_type='%s'",$domain, $result->casaa_path, $token);
				$value = db_fetch_object($query);
				if ($value->casaa_value) {
					return $value->casaa_value;
				}
			}
		}
	}
}

/**
* Implementation of hook_nodeapi()
* @global $script_written flag to prevent script from being included more than once per page load.
* @global $exception_found flag to indicate if an exception had been found.
*
* @param $tokens a list of the currently installed plugins.
*	@param $script_values the values from each plugin's settings for a prticular tax term.
*
* if an exception has not been found find if any of the plugins have any mapping settings for the 
* for the current node's taxonomy term. if so, call the plugin's output function and pass it the
* values.
*
* @todo add in the support for wildcard settings.
*/
function casaa_nodeapi(&$node, $op, $a3 = NULL, $a4 = NULL) {
	//$tokens = casaa_get_plugin_tokens_with_values(TRUE);
	$plugins = _casaa_load_plugins();
	switch ($op) {
		case 'load':
			break; 
		case 'view':
		 	global $exception_found;
			global $script_written;
				foreach ($plugins as $token => $values) {
					if (!$exception_found[$token]) {
						$results = array();	
						foreach((array)$node->taxonomy as $nid => $tid){
							$query = db_query("SELECT c.casaa_value FROM {casaa_context_mapper} c JOIN {casaa_terms} t ON c.aid = t.aid WHERE c.casaa_domain=%d AND c.casaa_type='%s' AND t.casaa_tid='%s'",$domain, $token, $tid->tid);
							$result = db_fetch_object($query);
							$results[] = $result;				
						}
						//test the weights and determine who's the dominant term in the group.
						if (!$results[0]) {
							$base_value = $results[1];
						}
						else {
							$base_value = $results[0];
						}
						if ($results) {
							foreach ($results as $result) {
								if ($result->casaa_weight < $base_weight->casaa_weight) {
									$script_value = $result->casaa_value;
								}
								else{
									$script_value = $base_value->casaa_value;
								}
							}
						}
						$hook_output = $token . '_output';
						if (function_exists($hook_output) && !$script_written[$token]) {
							$hook_output($script_value, unserialize($values['globals']));
						}
					}
				}

			break;			
	}
}

function casaa_footer()
{
	// check to ensure that any plugin which can generate decent output without mapping has been called
	$tokens = _casaa_load_plugins();
	foreach ($tokens as $token => $values)
	{
		$hook_output = $token . '_output';
		if (variable_get($token . '_has_defaults', FALSE) && !$script_written[$token] && function_exists($hook_output))
		{
			$hook_output(null);
		}
	}
}