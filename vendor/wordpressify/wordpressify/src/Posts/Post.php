<?php

namespace WP\Posts;


class Post extends PostObject {
	public $type = 'post';

	public function __construct( $postID = null ) {
		$this->defaults = [
			'post_title'   => ' ',
			'post_content' => ' ',
			'post_excerpt' => ' '

		];
		parent::__construct( $postID );
	}
}