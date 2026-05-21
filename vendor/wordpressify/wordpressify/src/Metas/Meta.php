<?php

namespace WP\Metas;


interface Meta {

	public function set_meta( $key, $value = null );

	public function get_meta( $key = '', $single = true );

}