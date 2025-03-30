-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Erstellungszeit: 09. Aug 2023 um 15:16
-- Server-Version: 10.3.29-MariaDB-0+deb10u1
-- PHP-Version: 7.3.29-1~deb10u1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Datenbank: `Belegung`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `BK_OnOff`
--

CREATE TABLE `BK_OnOff` (
  `id` int(11) NOT NULL,
  `tIdx` int(5) NOT NULL,
  `WTag` varchar(5) NOT NULL,
  `Geraet` varchar(5) NOT NULL,
  `Ein` time NOT NULL,
  `Aus` time NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Daten für Tabelle `BK_OnOff`
--

INSERT INTO `BK_OnOff` (`id`, `tIdx`, `WTag`, `Geraet`, `Ein`, `Aus`) VALUES
(1, 0, 'Mo', 'B', '14:30:00', '14:45:00'),
(2, 1, 'Di', 'B', '22:30:00', '23:00:00'),
(3, 2, 'Mi', 'B', '15:00:00', '15:15:00'),
(4, 3, 'Do', 'B', '22:45:00', '23:00:00'),
(5, 4, 'Fr', 'B', '23:00:00', '23:15:00'),
(6, 5, 'Sa', 'B', '14:30:00', '21:00:00'),
(7, 6, 'So', 'B', '22:15:00', '22:30:00'),
(8, 0, 'Mo', 'K', '14:30:00', '14:45:00'),
(9, 1, 'Di', 'K', '22:30:00', '22:45:00'),
(10, 2, 'Mi', 'K', '15:00:00', '15:15:00'),
(11, 3, 'Do', 'K', '22:45:00', '23:00:00'),
(12, 4, 'Fr', 'K', '23:00:00', '23:15:00'),
(13, 5, 'Sa', 'K', '12:00:00', '12:15:00'),
(14, 6, 'So', 'K', '22:15:00', '22:30:00');

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `BK_OnOff`
--
ALTER TABLE `BK_OnOff`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `BK_OnOff`
--
ALTER TABLE `BK_OnOff`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=15;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
