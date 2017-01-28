-- phpMyAdmin SQL Dump
-- version 3.4.3.2
-- http://www.phpmyadmin.net
--
-- Client: 127.0.0.1
-- Généré le : Ven 27 Janvier 2017 à 20:42
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
CREATE DATABASE `doxeo` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE `doxeo`;

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
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=4 ;

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
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=9 ;

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
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=29 ;

-- --------------------------------------------------------

--
-- Structure de la table `switch`
--

CREATE TABLE IF NOT EXISTS `switch` (
  `id` tinyint(4) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `status` varchar(4) COLLATE utf8_unicode_ci NOT NULL,
  `command` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=5 ;

-- --------------------------------------------------------

--
-- Structure de la table `switch_schedule`
--

CREATE TABLE IF NOT EXISTS `switch_schedule` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `switch_id` tinyint(4) NOT NULL,
  `start` datetime NOT NULL,
  `stop` datetime NOT NULL,
  `check_freebox` tinyint(1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `switch_id` (`switch_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=73 ;

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
  PRIMARY KEY (`id`)
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

--
-- Contraintes pour la table `switch_schedule`
--
ALTER TABLE `switch_schedule`
  ADD CONSTRAINT `switch_schedule_ibfk_1` FOREIGN KEY (`switch_id`) REFERENCES `switch` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
