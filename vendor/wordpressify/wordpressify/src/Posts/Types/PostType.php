<?php

namespace WP\Posts\Types;


class PostType {

	public function __construct( $type = 'post' ) {
		$this->type = $type;
	}

	public function __toString() {
		return $this->get_type();
	}

	public function get_type() {
		return $this->type;
	}

	public function posts( $args = [] ) {
		if ( post_type_exists( $this->get_type() ) ) {
			return get_posts( array_merge( [
				'post_type' => $this->get_type()
			], $args ) );
		}

		return false;
	}


}