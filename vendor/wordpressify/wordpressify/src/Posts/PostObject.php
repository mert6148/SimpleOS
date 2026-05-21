<?php

namespace WP\Posts;

use WP\Metas\Acf\Field;
use WP\Metas\Meta;
use WP\Posts\Types\PostType;

/**
 * Class PostObject
 * Assistant class to create or get posts
 */
abstract class PostObject implements \ArrayAccess, Meta, Field {
	/**
	 * @var $post array|\WP_Post
	 */
	public $post = [];

	public $type = 'post';

	public $params = [];

	public $defaults = [];


	public function __construct( $postID = null ) {
		$this->params = [
			'title'   => 'post_title',
			'content' => 'post_content',
			'status'  => 'post_status',
		];

		$this->type = new PostType( $this->type );
		if ( empty( $this->post ) ) {
			$this->post = (object) [];
		}

		$this->defaults = array_merge( array(
			'post_status' => 'publish',
			'post_author' => 0,
			'post_type'   => $this->type
		), $this->defaults );

		$this->post = get_post( $postID );
		if ( $this->post == null ) {
			$this->post = [];
		}
	}


	public function get_id() {
		return isset( $this->post->ID ) ? $this->post->ID : null;
	}


	/**
	 * @return $this|int|\WP_Error
	 */
	public function save() {
		if ( isset( $this->post->ID ) ) {
			$this->post->ID = $this->get_id();
			$this->update();
		} else {
			var_dump( array_merge( $this->defaults, $this->post ) );
			$insertResponse = wp_insert_post( array_merge( $this->defaults, $this->post ), true );
			if ( ! is_a( $insertResponse, 'WP_Error' ) ) {
				$this->post = get_post( $insertResponse );
			} else {
				return $insertResponse;
			}
		}

		return $this;
	}

	public function update() {
		wp_update_post( $this->post );
	}

	public function refresh() {
		$this->post = get_post( $this->post->ID );
	}


	private function filterOffset( $offset ) {
		if ( is_string( $offset ) ) {
			if ( array_key_exists( $offset, $this->params ) ) {
				$offset = $this->params[ $offset ];
			}
		}

		return $offset;
	}

	public function __get( $name ) {
		return $this->offsetGet( $name );
	}

	public function __set( $name, $value ) {
		$this->offsetSet( $name, $value );
	}

	public function offsetUnset( $offset ) {
		$offset = $this->filterOffset( $offset );
		if ( is_a( $this->post, 'WP_Post' ) ) {
			unset( $this->post->$offset );
		} else {
			unset( $this->post[ $offset ] );
		}

	}

	public function offsetSet( $offset, $value ) {
		$offset = $this->filterOffset( $offset );
		if ( is_a( $this->post, 'WP_Post' ) ) {
			$this->post->$offset = $value;
		} else {
			if ( is_null( $offset ) ) {
				$this->post[] = $value;
			} else {
				$this->post[ $offset ] = $value;
			}
		}
	}

	public function offsetGet( $offset ) {
		$offset = $this->filterOffset( $offset );
		if ( is_a( $this->post, 'WP_Post' ) ) {
			return $this->post->$offset;
		} else {
			return isset( $this->post[ $offset ] ) ? $this->post[ $offset ] : null;
		}
	}

	public function offsetExists( $offset ) {
		$offset = $this->filterOffset( $offset );
		if ( is_a( $this->post, 'WP_Post' ) ) {
			return isset( $this->post->$offset );
		} else {
			return isset( $this->post[ $offset ] );
		}
	}

	public function get_meta( $key = '', $single = true ) {
		return get_post_meta( $this->get_id(), $key, $single );
	}

	public function set_meta( $key, $value = null ) {
		return update_post_meta( $this->get_id(), $key, $value );
	}

	public static function has_acf() {
		return function_exists( 'get_field' );
	}

	public function get_field( $selector, $format_value = true ) {
		return self::has_acf() ? get_field( $selector, $this->get_id(), $format_value ) : null;
	}

	public function update_field( $selector, $value ) {
		return self::has_acf() ? update_field( $selector, $value, $this->get_id() ) : false;
	}

}