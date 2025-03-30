SELECT Name, Vorname, VZB, Chip_Val, Chip_lade_val FROM Mitglieder WHERE ((VZB = 'Y') AND (Chip_lade_val > 0));

SELECT DISTINCT m.Name, m.Vorname, m.Mitgl_Nr FROM Mitglieder m RIGHT JOIN CWTabelle c ON m.Mitgl_Nr = c.MitglNr WHERE ((c.BarChip = 'ChipLaden') AND ( c.Datum BETWEEN '2024-01-01' AND '2024-12-31')) ORDER BY m.Name ASC;

SELECT DISTINCT m.Name, m.Vorname, m.Mitgl_Nr, m.Chip_lade_val FROM Mitglieder m RIGHT JOIN CWTabelle c ON m.Mitgl_Nr = c.MitglNr WHERE ((c.BarChip = 'ChipLaden') AND ( c.Datum BETWEEN '2024-01-01' AND '2024-12-31'));