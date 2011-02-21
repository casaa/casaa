<?php
/*************************************************
 * @file: casaa.table.inc
 * @authors: geoff.maxey / tobby.hagler
 * 
 * Build and display the tables for all current
 * mapping settings and the configuration options 
 * through modal windows.
 **************************************************/

/**
 * Build the modal window to display the table.
 */
function casaa_build_tables_callback($js = FALSE) {
	$title = t('Table View');
	
	// Don't forget to sanitize!!!!!
	$domain = arg(6);
	$type = arg(7);
	
	$output = casaa_build_table($domain, $type);
	
	if ($js) {
    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_render($title, $output);
    // above command will exit().
  }
  else {
    drupal_set_title($title);
    return $output;
  }
}


/**
 * Pull the mappings for the domain and arrange 
 * them in rows for the table.
 * 
 * @todo: have the options links stay within the modal window.
 * @todo: condense the process to eliminate extraneous code.
 * @todo: lose a loop... lols
 */
function casaa_build_table($domain, $type) {
	$rows = array();
	$columns = array();
	
	$plugins = _casaa_load_plugins();
	
	if ($type == "paths") {
		$path_header = _casaa_build_header('path');
		$query = db_query("SELECT DISTINCT casaa_path FROM {casaa_paths}");
		while ($path = db_fetch_object($query)) {
			$paths[] = $path->casaa_path;
		}
		foreach ($paths as $path) {
			$columns = array();
			$columns[] = $path;
			foreach ($plugins as $plugin => $value) {
				$query = db_query("SELECT c.aid, c.casaa_value FROM {casaa_context_mapper} c 
					LEFT JOIN {casaa_paths} p ON c.aid = p.aid
					WHERE c.casaa_type = '%s' AND p.casaa_path = '%s' AND c.casaa_domain = %d
					", $plugin, $path, $domain);
					
				$value = db_fetch_object($query);
				$hook = $plugin . "_table_display_data";
				if (function_exists($hook)) {
					$display_data = $hook($value->casaa_value);
				}
				$columns[] = $display_data;
			}
			$pid = _casaa_get_path_id($path);
			$columns[] = l('Configure', 'admin/build/casaa/mappings/nojs/edit/' . $domain . '/path/' . $pid, array('attributes' => array('class' => 'ctools-use-ajax'))) . "&nbsp/&nbsp" . l('Delete', 'admin/build/casaa/mappings/nojs/delete/' . $domain . '/path/' . $pid, array('attributes' => array('class' => 'ctools-use-ajax')));
			$rows[] = $columns;
		}
		$output = theme_table($path_header, $rows);
	}
	else if ($type == "terms") {
		$term_header = _casaa_build_header('term');
		$query = db_query("SELECT DISTINCT c.casaa_tid, t.name FROM {casaa_terms} c
			LEFT JOIN {term_data} t ON c.casaa_tid = t.tid
		");
		while ($tid = db_fetch_object($query)) {
			$tids[$tid->casaa_tid] = $tid->name;
		}
		foreach ($tids as $tid => $term) {
			$columns = array();
			$columns[] = $term;
			foreach ($plugins as $plugin => $value) {
				$query = db_query("SELECT c.aid, c.casaa_value FROM {casaa_context_mapper} c
					LEFT JOIN {casaa_terms} t ON c.aid = t.aid
					WHERE c.casaa_type = '%s' AND t.casaa_tid = %d AND c.casaa_domain = %d
				", $plugin, $tid, $domain);
				
				$value = db_fetch_object($query);
				$hook = $plugin . "_table_display_data";
				if (function_exists($hook)) {
					$display_data = $hook($value->casaa_value);
				}
				$columns[] = $display_data;
			}
			$columns[] = l('Configure', 'admin/build/casaa/mappings/nojs/edit/' . $domain . '/term/' . $tid, array('attributes' => array('class' => 'ctools-use-ajax'))) . "&nbsp/&nbsp" . l('Delete', 'admin/build/casaa/mappings/nojs/delete/' . $domain . '/term/' . $tid, array('attributes' => array('class' => 'ctools-use-ajax')));
			$rows[] = $columns;
		}
		$output = theme_table($term_header, $rows);
	}
	
	return $output;
}

/**
 * Build the header row for the display table
 */ 
function _casaa_build_header($type) {
	if ($type == 'path') {
		$header[] = t('Paths');
	}
	else if ($type == 'term') {
		$header[] = t('Taxonomy Terms');
	}
	$plugins = _casaa_load_plugins();
	foreach ($plugins as $key => $values) {
		$header[] = $key;
	}
	return $header;
}

/**
 * Get a unique id for a path
 */ 
function _casaa_get_path_id($path) {
	$aid = db_fetch_object(db_query("SELECT aid FROM {casaa_paths} WHERE casaa_path='%s'", $path));

	return $aid->aid;	
}