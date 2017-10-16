-- phpMyAdmin SQL Dump
-- version 3.4.3.2
-- http://www.phpmyadmin.net
--
-- Client: 127.0.0.1
-- Généré le : Lun 16 Octobre 2017 à 20:49
-- Version du serveur: 5.5.15
-- Version de PHP: 5.3.8

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de données: `doxeo`
--

-- --------------------------------------------------------

--
-- Structure de la table `heater`
--

CREATE TABLE IF NOT EXISTS `heater` (
  `id` tinyint(4) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `command` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `mode` tinyint(1) NOT NULL,
  `cool_setpoint` float NOT NULL,
  `heat_setpoint` float NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=3 ;

-- --------------------------------------------------------

--
-- Structure de la table `heater_indicator`
--

CREATE TABLE IF NOT EXISTS `heater_indicator` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `heater_id` int(11) unsigned NOT NULL,
  `duration` int(11) NOT NULL,
  `start_date` datetime NOT NULL,
  `end_date` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=45 ;

-- --------------------------------------------------------

--
-- Structure de la table `schedule_event`
--

CREATE TABLE IF NOT EXISTS `schedule_event` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `heater_id` tinyint(4) NOT NULL,
  `setpoint` varchar(5) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  KEY `heater_id` (`heater_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=304 ;

-- --------------------------------------------------------

--
-- Structure de la table `schedule_occurrence`
--

CREATE TABLE IF NOT EXISTS `schedule_occurrence` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(11) unsigned NOT NULL,
  `start_date` datetime NOT NULL,
  `end_date` datetime NOT NULL,
  `recurrent_date` tinyint(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `event_id` (`event_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=738 ;

-- --------------------------------------------------------

--
-- Structure de la table `script`
--

CREATE TABLE IF NOT EXISTS `script` (
  `id` tinyint(4) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `status` varchar(4) COLLATE utf8_unicode_ci NOT NULL,
  `description` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `content` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=16 ;

-- --------------------------------------------------------

--
-- Structure de la table `sensor`
--

CREATE TABLE IF NOT EXISTS `sensor` (
  `id` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `cmd` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `value` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `switch`
--

CREATE TABLE IF NOT EXISTS `switch` (
  `id` varchar(25) COLLATE utf8_unicode_ci NOT NULL,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `status` varchar(4) COLLATE utf8_unicode_ci NOT NULL,
  `power_on_cmd` varchar(150) COLLATE utf8_unicode_ci NOT NULL,
  `power_off_cmd` varchar(150) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `temperature`
--

CREATE TABLE IF NOT EXISTS `temperature` (
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `temperature` float NOT NULL,
  UNIQUE KEY `date` (`date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Structure de la table `user`
--

CREATE TABLE IF NOT EXISTS `user` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(100) NOT NULL,
  `password` varchar(255) NOT NULL,
  `salt` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=8 ;

--
-- Contraintes pour les tables exportées
--

--
-- Contraintes pour la table `schedule_event`
--
ALTER TABLE `schedule_event`
  ADD CONSTRAINT `schedule_event_ibfk_1` FOREIGN KEY (`heater_id`) REFERENCES `heater` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Contraintes pour la table `schedule_occurrence`
--
ALTER TABLE `schedule_occurrence`
  ADD CONSTRAINT `schedule_occurrence_ibfk_1` FOREIGN KEY (`event_id`) REFERENCES `schedule_event` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
