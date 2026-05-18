<?php

namespace WP;


use WP\Admin\PostColumn;

final class WP {
	private static $instance = null;

	private function __construct() {

	}

	public static function getInstance() {
		if ( self::$instance == null ) {
			self::$instance = new self();
		}

		return self::$instance;
	}

	public function is_debug() {
		return defined( 'WP_DEBUG' ) ? WP_DEBUG : false;
	}
	

}