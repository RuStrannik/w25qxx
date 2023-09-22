# W25Qxx Universal Driver v2.0
![License](./img/license_bage_MIT.svg)
<!--[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](./LICENSE)-->

---
LAST UPDATE (09/2023):
An explanation as to why I no longer host my personal repositories on Github.

Github has [entered into an agreement with Microsoft](https://blogs.microsoft.com/blog/2018/06/04/microsoft-github-empowering-developers/) to be purchased.
Under this agreement, "GitHub will be led by CEO Nat Friedman, an open source veteran and founder of Xamarin, who will continue to report to Microsoft Cloud + AI Group Executive Vice President Scott Guthrie".

Here are some of the things the Microsoft Cloud + AI Group are up to:

* [Trying to win the JEDI contract, a 10 billion dollar contract with the Pentagon](https://about.bgov.com/blog/amazon-microsoft-battle-jedi-contract-d-c-metro-ad-war/)
* [Won a 6 year cloud computing agreement with 17 U.S. intelligence agencies](https://washingtontechnology.com/articles/2018/05/16/microsoft-ic-pact.aspx)
* [Wrote about their pride that Azure is mission critical to ICE](https://blogs.msdn.microsoft.com/azuregov/2018/01/24/federal-agencies-continue-to-advance-capabilities-with-azure-government/)
* [Entered the "Predictive Policing" market, using Chicago as their case study](https://enterprise.microsoft.com/en-us/articles/industries/citynext/safer-cities/predictive-policing-the-future-of-law-enforcement/)
  The Chicago PD is notoriously corrupt. The [Department of Justice investigation](https://www.justice.gov/opa/file/925846/download) of Chicago's police department found "a pattern or practice of unconstitutional use of force" and numerous violations of citizens Constitutional rights
* [GitHub contract to provide GitHub Enterprise Server to the Enforcement and Removal Operations (ERO) division of United States Immigrations and Customs Enforcement (ICE), which is actively committing numerous crimes and human rights violations, in contravention of both US and international law](https://github.com/taotetek/dear-github-2.0)
* [Unprecedented open-source software piracy at Github](https://githubcopilotlitigation.com/)
* Finally, I am just sick of Microsoft stuffing their policies down my throat through GitHub, so I decided to end all my collaboration with them and use combination of [self-hosted](https://git.serenity-island.net/sie-foss/w25qxx) and [public](https://codeberg.org/sie-foss/w25qxx) git repositories.

Efforts of open source software developers should not be used to help increase the market value and image of a company making billions of dollars enabling surveillance, violence, and human rights abuses.

I will keep my github account to communicate resons stated above, but I will not host my projects here and will put my best efforts to discourage others from doing so.

---

Modern rewrite of popular library.<br/>
Complete [example implementations](./examples) available for STM32F401 (BlackPill) and Orange PI Zero platforms.  
Heavily optimized code size, improved execution speed, minimized chip access instruction sequences and RAM usage.

## Feature summary

- Supported full spectrum of chip functionality (_fast-read_, _SFDP_, _memory block locking_, etc.)
- Production-ready code with advanced error detection and handling 
- RTOS-aware implementation 
    - Functions are reentrant and thread-safe
    - Embedded usage of user-defined optimized delay functions
    - Allowed mutexes to manage concurrent memory access*
- Implemented usage of some architecture-specific instructions for best performance (_like 'rev' aka 'bswap32()' for ARM_)
- Implemented selection of dynamic / static overwrite buffer (selected at compile time)
- Simple interfacing for any platform (see [examples](./examples))
- No external dependencies
- Smart overwriting function: skips erase-write cycle for blocks containing the same data
- Built-in verification of written data for detecting bad blocks

## Usage example

``` C
#include "w25qxx.h"

extern w25qxx_dev_t spi_flash_chip1; // allocated and filled with platform-specific calls and chip-specific bindings  elsewhere

int w25qxx_test(void) {
    int ret;
    char buf[W25QXX_UID_LEN] = {0}; // W25QXX_UID_LEN == 8
    
    ret = w25qxx_init(&spi_flash_chip1);
    if (ret != W25QXX_RET_SUCCESS) { // W25QXX_RET_SUCCESS == 0 
        printf("w25qxx_init(): Failed with code: %02X\r\n", ret); 
        return ret; 
    };

    ret = w25qxx_get_unique_id(&spi_flash_chip1, buf);
    if (ret) { printf("w25qxx_get_unique_id(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret)); return ret; };

    char uid_str[W25QXX_UID_LEN * 2 + 1] = {0};
    ret = bytes2str(uid_str, buf, W25QXX_UID_LEN);
    if (ret != W25QXX_UID_LEN) { printf("bytes2str(): Failed somehow!\r\n"); return 1; };
    printf("Chip 1 UID: '%s'\r\n", uid_str);

    const int val = 123;
    const unsigned int val_addr = 0x00000000;
    
    ret = w25qxx_overwrite_verify(&spi_flash_chip1, val_addr, &val, sizeof(val));
    if (ret) { printf("w25qxx_overwrite_verify(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret));  return ret; };

    ret = w25qxx_read_fast(&spi_flash_chip1, val_addr, buf, sizeof(ret));
    if (ret) { printf("w25qxx_read_fast(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret));  return ret; };

    ret = *(int *)buf;
    printf("Chip 1 read data: %d\r\n", ret);

    return 0;
};

```
