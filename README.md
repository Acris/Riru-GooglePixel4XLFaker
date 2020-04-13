# Riru - Google Pixel 4 XL Faker

![License GPL-3.0](https://img.shields.io/badge/license-GPLv3.0-green.svg)

Fake as Google Pixel 4 XL by hook system_property_get.

Requires Riru - Core v19 or above installed.

## What does this module do	

By default, `__system_property_get` (`android::base::GetProperty` on Pie+) will be hooked in packages except `com.oneplus.camera` and `com.oneplus.gallery` with value map below	

* `ro.product.vendor.manufacturer` -> `Google`
* `ro.product.vendor.brand` -> `google`
* `ro.product.vendor.model` -> `Pixel 4 XL`
* `ro.product.vendor.name` -> `coral`
* `ro.product.vendor.device` -> `coral`
