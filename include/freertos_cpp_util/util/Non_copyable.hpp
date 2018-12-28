/**
 * @brief Non_copyable
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

class Non_copyable
{
protected:
	Non_copyable() = default;
	~Non_copyable() = default;

	Non_copyable(const Non_copyable& rhs) = delete;
	Non_copyable& operator=(const Non_copyable& rhs) = delete;
};
