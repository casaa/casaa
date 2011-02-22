<?php
/**************************************************************
 * casaa_main.tpl.php
 * 
 * @params:
 *  - $domains - array of available domains and their options
 *  - $options
 *  - $active_domain - 
 **************************************************************/
?>
<?php //var_dump($active_domain); ?>
<?php drupal_add_css(drupal_get_path('module', 'casaa') . "/themes/css/casaa.css", 'module', 'all', TRUE); ?>
<div class="casaa-settings">
	<div class="casaa-left-rail">
		<?php if (is_array($domains) && !empty($domains)) : ?>
			<ul class="casaa-plugin-list">
				<?php $i=0; foreach ($domains as $domain) :?>
					<li id="casaa-<?php ($domain['domain_id'] == $active_domain) ? print 'active' : print $i++; ?>"><?php print l($domain['sitename'], 'admin/build/casaa/main/' . $domain['domain_id']);?></li>
				<?php endforeach; ?>
			</ul>
		<?php else: ?>
			<span class="casaa-message">Something ain't right. Call your administrator!</span>
		<?php endif; ?>
	</div>
	<div class="casaa-main-content">
		<?php print $options; ?>
		<div class="casaa-left-column">
			<span class="casaa-header">Manage</span>
			<ul class="casaa-plugin-list">
				<li><?php print l('View Term Mappings', 'admin/build/casaa/mappings/nojs/view/' . $active_domain . '/terms', array(
			    'attributes' => array('class' => 'ctools-use-modal'))); ?></li>
				<li><?php print l('View Path Mappings', 'admin/build/casaa/mappings/nojs/view/' . $active_domain . '/paths', array(
					    'attributes' => array('class' => 'ctools-use-modal'))); ?></li>
			</ul>
		</div>
		<div class="casaa-left-column">
			<span class="casaa-header">Create</span>
			<ul class="casaa-plugin-list">
				<?php
					ctools_include('modal');
					ctools_modal_add_js();
				?>
				<li><?php print l('Add Term Mapping', 'admin/build/casaa/mappings/nojs/add/' . $active_domain . '/terms'/*, array(
			    'attributes' => array('class' => 'ctools-use-modal'))*/); ?></li>
				<li><?php print l('Add Path Mapping', 'admin/build/casaa/mappings/nojs/add/' . $active_domain . '/paths'/*, array(
					    'attributes' => array('class' => 'ctools-use-modal'))*/); ?></li>
			</ul>
		</div>
		<div class="casaa-right-column">
			<span class="casaa-header">Find</span>
		</div>
		<div class="casaa-right-column">
			<span class="casaa-header">Move</span>
		</div>
	</div>
</div>