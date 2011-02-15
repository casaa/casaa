<?php
/**
 * CASAA Context Path Condition
 * @file: casaa_path.context.inc
 * @author: geoff.maxey / tobby.hagler
 * 
 * Control the conditions and reactions for outputting
 * CASAA plug-in data via the Context Module
 */ 

/**
 * 
 */
class casaa_path_context_condition extends context_condition_path {
	
	function condition_values() {
		return array(1 => 'Enable this Context');
	}
	
	function execute() {
		var_dump('shit');
		global $exception_found;
		global $script_written;
		
		if ($this->condition_used()) {
			if (drupal_is_front_page()) {
				$current_path = '[front]';
			}
			else {
				$current_path = array(drupal_get_path_alias($_GET['q']));
      	if ($current_path != $_GET['q']) {
       		$current_path[] = $_GET['q'];
      	}
			}
			$plugins = _casaa_load_plugins();
			if (module_exists('domain')) {
				$domain = domain_get_domain();
			}
			else {
				$domain['domain_id'] = 0;
			}
      foreach ($this->get_contexts() as $context) {
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
		}
	}
	
	function casaa_path_match($path, $plugin, $domain) {
		var_dump('checking paths');die;
		return TRUE;
	}
}