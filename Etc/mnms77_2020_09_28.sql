/*
Navicat MySQL Data Transfer

Source Server         : Local
Source Server Version : 50051
Source Host           : localhost:3306
Source Database       : mnms77

Target Server Type    : MYSQL
Target Server Version : 50051
File Encoding         : 65001

Date: 2020-09-28 23:02:07
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `AccountID` int(11) NOT NULL auto_increment,
  `AccountName` varchar(64) NOT NULL default '',
  `Password` varchar(255) NOT NULL default '',
  `SecondPassword` varchar(255) default '',
  `NexonCash` int(11) default '0',
  `MaplePoint` int(11) default '0',
  `NXPoint3` int(11) default '0',
  `RegisterDate` datetime default NULL,
  `Gender` int(3) default '-1',
  PRIMARY KEY  (`AccountID`,`AccountName`),
  KEY `AccountID` (`AccountID`)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for bbscomment
-- ----------------------------
DROP TABLE IF EXISTS `bbscomment`;
CREATE TABLE `bbscomment` (
  `WorldID` int(11) NOT NULL,
  `GuildID` int(11) NOT NULL default '0',
  `EntryID` int(11) NOT NULL default '0',
  `SN` int(11) NOT NULL,
  `Comment` varchar(255) default '',
  `Date` bigint(21) default '0',
  `CharacterID` int(11) default '0',
  PRIMARY KEY  (`GuildID`,`EntryID`,`SN`,`WorldID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for bbsentry
-- ----------------------------
DROP TABLE IF EXISTS `bbsentry`;
CREATE TABLE `bbsentry` (
  `WorldID` int(11) NOT NULL default '0',
  `GuildID` int(11) NOT NULL default '0',
  `EntryID` int(11) NOT NULL default '0',
  `Emoticon` int(11) default '0',
  `CharacterID` int(11) default '0',
  `Title` varchar(64) default '',
  `Text` varchar(255) default '',
  `Date` bigint(21) default '0',
  `IsNotice` int(5) default '0',
  PRIMARY KEY  (`GuildID`,`EntryID`,`WorldID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cashiteminfo
-- ----------------------------
DROP TABLE IF EXISTS `cashiteminfo`;
CREATE TABLE `cashiteminfo` (
  `SN` int(11) NOT NULL auto_increment,
  `CashItemSN` bigint(21) NOT NULL default '0',
  `AccountID` int(11) NOT NULL default '0',
  `CharacterID` int(11) NOT NULL default '0',
  `ItemID` int(11) default '0',
  `CommodityID` int(11) default '0',
  `Number` int(6) default '1',
  `BuyCharacterID` varchar(32) default '',
  `DateExpire` bigint(21) default '0',
  `PaybackRate` int(11) default '0',
  `DiscountRate` decimal(10,0) default '0',
  `OrderNo` int(11) default '0',
  `ProductNo` int(11) default '0',
  `Refundable` int(1) default '0',
  `SourceFlag` int(1) default '0',
  `StoreBank` int(1) default '0',
  `OptExpireDate` bigint(21) default '0',
  `OptGrade` int(11) default '0',
  `Opt1` int(11) default '0',
  `Opt2` int(11) default '0',
  `Opt3` int(11) default '0',
  PRIMARY KEY  (`SN`,`AccountID`,`CashItemSN`,`CharacterID`),
  KEY `p2` (`CashItemSN`)
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cashitem_bundle
-- ----------------------------
DROP TABLE IF EXISTS `cashitem_bundle`;
CREATE TABLE `cashitem_bundle` (
  `SN` int(11) default NULL,
  `CashItemSN` bigint(21) NOT NULL,
  `CharacterID` int(11) default NULL,
  `ItemID` int(11) default '0',
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`CashItemSN`),
  KEY `CID` (`CharacterID`),
  CONSTRAINT `CS_IC` FOREIGN KEY (`CashItemSN`) REFERENCES `cashiteminfo` (`CashItemSN`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cashitem_eqp
-- ----------------------------
DROP TABLE IF EXISTS `cashitem_eqp`;
CREATE TABLE `cashitem_eqp` (
  `SN` int(11) NOT NULL default '0',
  `CashItemSN` bigint(21) NOT NULL default '-1',
  `ItemID` int(11) default NULL,
  `CharacterID` int(11) NOT NULL,
  `Number` int(11) default NULL,
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  `RUC` int(11) default '0',
  `CUC` int(11) default '0',
  `Cuttable` int(11) default '0',
  `I_STR` int(11) default '0',
  `I_DEX` int(11) default '0',
  `I_INT` int(11) default '0',
  `I_LUK` int(11) default '0',
  `I_MaxHP` int(11) default '0',
  `I_MaxMP` int(11) default '0',
  `I_PAD` int(11) default '0',
  `I_MAD` int(11) default '0',
  `I_PDD` int(11) default '0',
  `I_MDD` int(11) default '0',
  `I_ACC` int(11) default '0',
  `I_EVA` int(11) default '0',
  `I_Speed` int(11) default '0',
  `I_Craft` int(11) default '0',
  `I_Jump` int(11) default '0',
  PRIMARY KEY  (`CashItemSN`),
  KEY `CS` (`CashItemSN`),
  KEY `CID` (`CharacterID`),
  CONSTRAINT `CS_CE` FOREIGN KEY (`CashItemSN`) REFERENCES `cashiteminfo` (`CashItemSN`) ON DELETE CASCADE ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cashitem_pet
-- ----------------------------
DROP TABLE IF EXISTS `cashitem_pet`;
CREATE TABLE `cashitem_pet` (
  `SN` int(11) NOT NULL,
  `CashItemSN` bigint(21) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `PetAttribute` int(11) default '0',
  `POS` int(11) default '0',
  `Level` int(3) default '0',
  `Repleteness` int(3) default '0',
  `Tameness` int(6) default '0',
  `PetName` varchar(18) default '',
  `RemainLife` int(11) default '0',
  `ActiveState` int(1) default '0',
  `AutoBuffSkill` int(11) default '0',
  `PetHue` int(11) default '0',
  `GiantRate` int(6) default '0',
  `PetSkill` int(6) default '0',
  PRIMARY KEY  (`CashItemSN`),
  KEY `CID` (`CharacterID`),
  CONSTRAINT `CS_CP` FOREIGN KEY (`CashItemSN`) REFERENCES `cashiteminfo` (`CashItemSN`) ON DELETE CASCADE ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for character
-- ----------------------------
DROP TABLE IF EXISTS `character`;
CREATE TABLE `character` (
  `CharacterID` int(11) NOT NULL auto_increment,
  `AccountID` int(11) default NULL,
  `WorldID` int(11) default NULL,
  `CharacterName` varchar(15) character set big5 NOT NULL default '',
  `GuildID` int(11) default '0',
  `FieldID` int(11) default '0',
  `FriendMaxNum` int(11) default '25',
  `ActiveEffectItemID` int(11) default '0',
  `GradeCode` int(3) default '0',
  PRIMARY KEY  (`CharacterID`,`CharacterName`),
  KEY `CharacterID` (`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=2049 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for characteravatar
-- ----------------------------
DROP TABLE IF EXISTS `characteravatar`;
CREATE TABLE `characteravatar` (
  `SN` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) default NULL,
  `Hair` int(11) default NULL,
  `Face` int(11) default NULL,
  `Skin` int(11) default NULL,
  PRIMARY KEY  (`SN`)
) ENGINE=MyISAM AUTO_INCREMENT=26 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for characterlevel
-- ----------------------------
DROP TABLE IF EXISTS `characterlevel`;
CREATE TABLE `characterlevel` (
  `CharacterID` int(11) NOT NULL,
  `Level` int(11) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for charactermoney
-- ----------------------------
DROP TABLE IF EXISTS `charactermoney`;
CREATE TABLE `charactermoney` (
  `CharacterID` int(11) NOT NULL,
  `Money` int(11) default '0',
  PRIMARY KEY  (`CharacterID`),
  KEY `CID` USING BTREE (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for characterslotcount
-- ----------------------------
DROP TABLE IF EXISTS `characterslotcount`;
CREATE TABLE `characterslotcount` (
  `CharacterID` int(11) NOT NULL,
  `EquipSlot` int(3) NOT NULL,
  `ConSlot` int(3) NOT NULL,
  `InstallSlot` int(3) NOT NULL,
  `EtcSlot` int(3) NOT NULL,
  `CashSlot` int(3) NOT NULL,
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for characterstat
-- ----------------------------
DROP TABLE IF EXISTS `characterstat`;
CREATE TABLE `characterstat` (
  `SN` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) NOT NULL,
  `Exp` bigint(20) default '0',
  `HP` int(11) default '0',
  `MP` int(11) default '0',
  `MaxHP` int(11) default '0',
  `MaxMP` int(11) default '0',
  `Gender` int(1) default '0',
  `Job` int(11) default '0',
  `SubJob` int(11) default '0',
  `Str` int(11) default NULL,
  `Dex` int(11) default '0',
  `Int_` int(11) default '0',
  `Luk` int(11) default '0',
  `SP` varchar(100) default '0,0,0,0,0,0,0,0,0,0,0',
  `AP` int(11) default '0',
  `POP` int(11) default '0',
  `CharismaEXP` int(11) default '0',
  `InsightEXP` int(11) default '0',
  `WillEXP` int(11) default '0',
  `SenseEXP` int(11) default '0',
  `CharmEXP` int(11) default '0',
  `Hair` int(11) default '0',
  `Skin` int(11) default '0',
  `Face` int(11) default '0',
  `FaceMark` int(11) default '0',
  PRIMARY KEY  (`SN`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=72 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for entrustedshop
-- ----------------------------
DROP TABLE IF EXISTS `entrustedshop`;
CREATE TABLE `entrustedshop` (
  `CharacterID` int(11) NOT NULL,
  `Money` bigint(22) default '0',
  PRIMARY KEY  (`CharacterID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for entrustedshop_con
-- ----------------------------
DROP TABLE IF EXISTS `entrustedshop_con`;
CREATE TABLE `entrustedshop_con` (
  `CharacterID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  `Locked` int(1) default '0',
  PRIMARY KEY  (`CharacterID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for entrustedshop_eqp
-- ----------------------------
DROP TABLE IF EXISTS `entrustedshop_eqp`;
CREATE TABLE `entrustedshop_eqp` (
  `CharacterID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  `Locked` int(1) default '0',
  PRIMARY KEY  (`CharacterID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for entrustedshop_etc
-- ----------------------------
DROP TABLE IF EXISTS `entrustedshop_etc`;
CREATE TABLE `entrustedshop_etc` (
  `CharacterID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  `Locked` int(1) default '0',
  PRIMARY KEY  (`CharacterID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for entrustedshop_ins
-- ----------------------------
DROP TABLE IF EXISTS `entrustedshop_ins`;
CREATE TABLE `entrustedshop_ins` (
  `CharacterID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  `Locked` int(1) default '0',
  PRIMARY KEY  (`CharacterID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for friend
-- ----------------------------
DROP TABLE IF EXISTS `friend`;
CREATE TABLE `friend` (
  `CharacterID` int(11) NOT NULL,
  `WorldID` int(11) default '0',
  `FriendID` int(11) NOT NULL default '0',
  `FriendName` varchar(20) default '',
  `Flag` int(1) default '0',
  PRIMARY KEY  (`CharacterID`,`FriendID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for funckeymapped
-- ----------------------------
DROP TABLE IF EXISTS `funckeymapped`;
CREATE TABLE `funckeymapped` (
  `CharacterID` int(11) NOT NULL,
  `Key` int(11) NOT NULL,
  `Type` int(3) default '0',
  `Value` int(11) default '0',
  PRIMARY KEY  (`CharacterID`,`Key`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for guildinfo
-- ----------------------------
DROP TABLE IF EXISTS `guildinfo`;
CREATE TABLE `guildinfo` (
  `GuildID` int(11) NOT NULL default '0',
  `WorldID` int(11) NOT NULL,
  `GuildName` varchar(64) default '',
  `MaxMemberNum` int(11) default '0',
  `MarkBg` int(6) default '0',
  `MarkBgColor` int(3) default '0',
  `Mark` int(6) default '0',
  `MarkColor` int(3) default '0',
  `Notice` varchar(255) default '',
  `Point` int(11) default '0',
  `GradeName1` varchar(10) default '',
  `GradeName2` varchar(10) default '',
  `GradeName3` varchar(10) default '',
  `GradeName4` varchar(10) default '',
  `GradeName5` varchar(10) default '',
  PRIMARY KEY  (`GuildID`,`WorldID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for guildmember
-- ----------------------------
DROP TABLE IF EXISTS `guildmember`;
CREATE TABLE `guildmember` (
  `GuildID` int(11) NOT NULL,
  `WorldID` int(11) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `CharacterName` varchar(32) default '',
  `Job` int(6) default '0',
  `Level` int(3) default '0',
  `Grade` int(2) default '3',
  `Contribution` int(11) default '0',
  PRIMARY KEY  (`GuildID`,`CharacterID`,`WorldID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemexpired_bundle
-- ----------------------------
DROP TABLE IF EXISTS `itemexpired_bundle`;
CREATE TABLE `itemexpired_bundle` (
  `ItemSN` int(11) NOT NULL default '0',
  `CashItemSN` bigint(21) NOT NULL,
  `CharacterID` int(11) default NULL,
  `ItemID` int(11) default '0',
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`CashItemSN`,`ItemSN`),
  KEY `CID` (`CharacterID`),
  CONSTRAINT `itemexpired_bundle_ibfk_1` FOREIGN KEY (`CashItemSN`) REFERENCES `cashiteminfo` (`CashItemSN`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemexpired_eqp
-- ----------------------------
DROP TABLE IF EXISTS `itemexpired_eqp`;
CREATE TABLE `itemexpired_eqp` (
  `ItemSN` int(11) NOT NULL default '0',
  `CashItemSN` bigint(21) NOT NULL default '-1',
  `ItemID` int(11) default NULL,
  `CharacterID` int(11) NOT NULL,
  `Number` int(11) default NULL,
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  `RUC` int(11) default '0',
  `CUC` int(11) default '0',
  `Cuttable` int(11) default '0',
  `I_STR` int(11) default '0',
  `I_DEX` int(11) default '0',
  `I_INT` int(11) default '0',
  `I_LUK` int(11) default '0',
  `I_MaxHP` int(11) default '0',
  `I_MaxMP` int(11) default '0',
  `I_PAD` int(11) default '0',
  `I_MAD` int(11) default '0',
  `I_PDD` int(11) default '0',
  `I_MDD` int(11) default '0',
  `I_ACC` int(11) default '0',
  `I_EVA` int(11) default '0',
  `I_Speed` int(11) default '0',
  `I_Craft` int(11) default '0',
  `I_Jump` int(11) default '0',
  KEY `CS` (`CashItemSN`),
  KEY `CID` (`CharacterID`),
  CONSTRAINT `itemexpired_eqp_ibfk_1` FOREIGN KEY (`CashItemSN`) REFERENCES `cashiteminfo` (`CashItemSN`) ON DELETE CASCADE ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemlocker
-- ----------------------------
DROP TABLE IF EXISTS `itemlocker`;
CREATE TABLE `itemlocker` (
  `SN` int(11) NOT NULL auto_increment,
  `CashItemSN` bigint(21) NOT NULL,
  `AccountID` int(11) NOT NULL default '0',
  `Type` int(1) default '1',
  `Locked` int(1) default '0',
  PRIMARY KEY  (`SN`,`CashItemSN`,`AccountID`),
  KEY `CashItemSN` (`CashItemSN`),
  CONSTRAINT `CS` FOREIGN KEY (`CashItemSN`) REFERENCES `cashiteminfo` (`CashItemSN`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_con
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_con`;
CREATE TABLE `itemslot_con` (
  `SN` int(11) NOT NULL,
  `ItemSN` bigint(21) NOT NULL,
  `ItemID` int(11) default '0',
  `CharacterID` int(11) NOT NULL,
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`ItemSN`),
  KEY `CID` USING BTREE (`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=1032 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_eqp
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_eqp`;
CREATE TABLE `itemslot_eqp` (
  `SN` int(11) NOT NULL default '0',
  `ItemSN` bigint(21) NOT NULL,
  `ItemID` int(11) default NULL,
  `CharacterID` int(11) NOT NULL,
  `Number` int(11) default NULL,
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  `RUC` int(11) default '0',
  `CUC` int(11) default '0',
  `Cuttable` int(11) default '0',
  `I_STR` int(11) default '0',
  `I_DEX` int(11) default '0',
  `I_INT` int(11) default '0',
  `I_LUK` int(11) default '0',
  `I_MaxHP` int(11) default '0',
  `I_MaxMP` int(11) default '0',
  `I_PAD` int(11) default '0',
  `I_MAD` int(11) default '0',
  `I_PDD` int(11) default '0',
  `I_MDD` int(11) default '0',
  `I_ACC` int(11) default '0',
  `I_EVA` int(11) default '0',
  `I_Speed` int(11) default '0',
  `I_Craft` int(11) default '0',
  `I_Jump` int(11) default '0',
  PRIMARY KEY  (`ItemSN`),
  KEY `CID` USING BTREE (`CharacterID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_etc
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_etc`;
CREATE TABLE `itemslot_etc` (
  `SN` int(11) NOT NULL,
  `ItemSN` bigint(21) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default NULL,
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`ItemSN`),
  KEY `CID` USING BTREE (`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=671 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for itemslot_ins
-- ----------------------------
DROP TABLE IF EXISTS `itemslot_ins`;
CREATE TABLE `itemslot_ins` (
  `SN` int(11) NOT NULL,
  `ItemSN` bigint(21) NOT NULL,
  `CharacterID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `Number` int(11) default '1',
  `ExpireDate` bigint(21) default '-1',
  `Title` varchar(18) default '',
  `Attribute` int(11) default '0',
  `POS` int(11) default '0',
  PRIMARY KEY  (`ItemSN`),
  KEY `CID` USING BTREE (`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=206 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for mobrewards
-- ----------------------------
DROP TABLE IF EXISTS `mobrewards`;
CREATE TABLE `mobrewards` (
  `id` bigint(20) NOT NULL auto_increment,
  `dropperid` int(11) NOT NULL,
  `itemid` int(11) NOT NULL default '0',
  `minimum_quantity` int(11) NOT NULL default '1',
  `maximum_quantity` int(11) NOT NULL default '1',
  `questid` int(11) NOT NULL default '0',
  `chance` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  KEY `mobid` (`dropperid`)
) ENGINE=MyISAM AUTO_INCREMENT=45197 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for questrecord
-- ----------------------------
DROP TABLE IF EXISTS `questrecord`;
CREATE TABLE `questrecord` (
  `ID` int(11) NOT NULL auto_increment,
  `CharacterID` int(11) NOT NULL default '0',
  `QuestID` int(11) default '0',
  `State` int(11) default '0',
  `Time` bigint(21) default '0',
  `StrRecord` varchar(128) default '',
  `MobRecord` varchar(255) default '',
  PRIMARY KEY  (`ID`,`CharacterID`)
) ENGINE=MyISAM AUTO_INCREMENT=2117107 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for shop
-- ----------------------------
DROP TABLE IF EXISTS `shop`;
CREATE TABLE `shop` (
  `SN` int(11) NOT NULL auto_increment,
  `NpcID` int(11) NOT NULL,
  `ItemID` int(11) default '0',
  `Price` int(10) default '0',
  `TokenItemID` int(11) default '0',
  `TokenPrice` int(11) default '0',
  `Quantity` int(6) default '0',
  `MaxPerSlot` int(11) default '0',
  `TabIndex` int(11) default '0',
  `LevelLimited` int(11) default '0',
  `Period` bigint(21) default '0',
  `ShowLevMin` int(11) default '0',
  `ShowLevMax` int(11) default '0',
  `ftSellStart` bigint(21) default '94354848000000000',
  `ftSellEnd` bigint(21) default '150842304000000000',
  PRIMARY KEY  (`SN`,`NpcID`)
) ENGINE=MyISAM AUTO_INCREMENT=7098 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for skillrecord
-- ----------------------------
DROP TABLE IF EXISTS `skillrecord`;
CREATE TABLE `skillrecord` (
  `CharacterID` int(11) NOT NULL,
  `SkillID` int(11) NOT NULL,
  `SLV` int(11) default NULL,
  `MasterLevel` int(11) default NULL,
  `Expired` bigint(20) default NULL,
  PRIMARY KEY  (`CharacterID`,`SkillID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for trunk
-- ----------------------------
DROP TABLE IF EXISTS `trunk`;
CREATE TABLE `trunk` (
  `AccountID` int(11) NOT NULL,
  `SlotCount` int(11) default '4',
  `Money` bigint(22) default '0',
  PRIMARY KEY  (`AccountID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for trunk_con
-- ----------------------------
DROP TABLE IF EXISTS `trunk_con`;
CREATE TABLE `trunk_con` (
  `AccountID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  PRIMARY KEY  (`AccountID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for trunk_eqp
-- ----------------------------
DROP TABLE IF EXISTS `trunk_eqp`;
CREATE TABLE `trunk_eqp` (
  `AccountID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  PRIMARY KEY  (`AccountID`,`SN`),
  KEY `eq` (`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for trunk_etc
-- ----------------------------
DROP TABLE IF EXISTS `trunk_etc`;
CREATE TABLE `trunk_etc` (
  `AccountID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  PRIMARY KEY  (`AccountID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for trunk_ins
-- ----------------------------
DROP TABLE IF EXISTS `trunk_ins`;
CREATE TABLE `trunk_ins` (
  `AccountID` int(11) NOT NULL,
  `SN` bigint(21) NOT NULL default '0',
  PRIMARY KEY  (`AccountID`,`SN`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
