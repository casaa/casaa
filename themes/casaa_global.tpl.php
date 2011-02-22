<?php
/******************************************************
 * casaa_global.tpl.php
 * 
 * @param: 
 *  - $plugins - an array of registered plug-ins and
 * 							 their global values
 *  - $active_plugin - The plug-in to view values for.
 *  - $plugin_form - The form for the selected plugin.
 ******************************************************/
?>
<?php drupal_add_css(drupal_get_path('module', 'casaa') . '/themes/css/casaa.css', 'module', 'all', TRUE); ?>

<div class="casaa-left-rail">
	<?php if (is_array($plugins) && !empty($plugins)) : ?>
		<ul class="casaa-plugin-list">	
			<?php $i = 0;	foreach ($plugins as $key => $plugin) : ?>
				<?php if (!$active_plugin && $i == 0) {$active_plugin = $key;} ?>
				<li id="casaa-<?php ($active_plugin == $key)? print 'active' : print $i++; ?>"><?php print l($plugin['name'], 'admin/build/casaa/globals/' . $key); ?></li>
			<?php endforeach;?>
		</ul>
	<?php else: ?>
		<span class="casaa-message">There are no plug-ins installed.</span>
	<?php endif;?>
</div>
<div class="casaa-main-content">
	<div class="casaa-status">
		<?php print drupal_get_form('casaa_status_form');?>
	</div>
	<?php
		print $plugin_form;
	?>
</div>