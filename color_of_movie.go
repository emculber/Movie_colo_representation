package main

import (
	"fmt"
	"image"
	"log"
	"math"
	"os"

	_ "image/jpeg"
)

func main() {

	// Decode the JPEG data. If reading from file, create a reader with
	//
	reader, err := os.Open("test.jpg")
	if err != nil {
		log.Fatal(err)
	}
	defer reader.Close()
	// reader := base64.NewDecoder(base64.StdEncoding, strings.NewReader(data))
	m, _, err := image.Decode(reader)
	if err != nil {
		log.Fatal(err)
	}
	bounds := m.Bounds()

	// Calculate a 16-bin histogram for m's red, green, blue and alpha components.
	//
	// An image's bounds do not necessarily start at (0, 0), so the two loops start
	// at bounds.Min.Y and bounds.Min.X. Looping over Y first and X second is more
	// likely to result in better memory access patterns than X first and Y second.
	var histogram [16][4]int
	var pixel_count uint32 = 0
	var total_red, total_green, total_blue uint32 = 0, 0, 0
	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			r, g, b, a := m.At(x, y).RGBA()
			// A color's RGBA method returns values in the range [0, 65535].
			// Shifting by 12 reduces this to the range [0, 15].
			histogram[r>>12][0]++
			histogram[g>>12][1]++
			histogram[b>>12][2]++
			histogram[a>>12][3]++

			red, green, blue, _ := m.At(x, y).RGBA()
			total_red += (red * red)
			total_green += (green * green)
			total_blue += (blue * blue)
			pixel_count++
		}
	}

	// Print the results.
	fmt.Printf("%-14s %6s %6s %6s %6s\n", "bin", "red", "green", "blue", "alpha")
	for i, x := range histogram {
		fmt.Printf("0x%04x-0x%04x: %6d %6d %6d %6d\n", i<<12, (i+1)<<12-1, x[0], x[1], x[2], x[3])
	}

	fmt.Println("Image bounds:(", bounds.Min.X, ",", bounds.Min.Y, ") (", bounds.Max.X, ",", bounds.Max.Y, ")")
	fmt.Println(m.At(0, 0).RGBA())
	fmt.Println(m.At(0, 0))
	fmt.Println(pixel_count)

	average_color_red := math.Sqrt(float64(total_red / pixel_count))
	average_color_green := math.Sqrt(float64(total_green / pixel_count))
	average_color_blue := math.Sqrt(float64(total_blue / pixel_count))

	fmt.Println("Average {", average_color_red, ",", average_color_green, ",", average_color_blue, "}")

	//NewColor = sqrt((R1^2+R2^2)/2),sqrt((G1^2+G2^2)/2),sqrt((B1^2+B2^2)/2)
}
