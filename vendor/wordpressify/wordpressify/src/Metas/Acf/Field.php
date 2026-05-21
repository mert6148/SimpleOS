<?php

namespace WP\Metas\Acf;


interface Field {

	public static function has_acf();

	public function get_field( $selector, $format_value = true );

	public function update_field( $selector, $value );

}