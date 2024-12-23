// mnist.jsx
import React, { useState, useEffect } from 'react';
import { createRoot } from 'react-dom/client';

const MNISTDigit = ({ imageData }) => {
    const canvasRef = React.useRef(null);

    React.useEffect(() => {
        const canvas = canvasRef.current;
        const ctx = canvas.getContext('2d');
        const imageDataObj = ctx.createImageData(28, 28);

        for (let i = 0; i < imageData.length; i++) {
            imageDataObj.data[i * 4] = imageData[i] * 255;
            imageDataObj.data[i * 4 + 1] = imageData[i] * 255;
            imageDataObj.data[i * 4 + 2] = imageData[i] * 255;
            imageDataObj.data[i * 4 + 3] = 255;
        }

        ctx.putImageData(imageDataObj, 0, 0);
    }, [imageData]);

    return (
        <canvas
            ref={canvasRef}
            width={28}
            height={28}
            className="w-24 h-24 border border-gray-300 bg-white"
            style={{ imageRendering: 'pixelated' }}
        />
    );
};

const MNISTGrid = () => {
    const [digits, setDigits] = useState(window.initialMNISTData || []);
    const [isLoading, setIsLoading] = useState(false);

    const handleShuffle = async () => {
        setIsLoading(true);
        try {
            const response = await fetch('/api/shuffle');
            const data = await response.json();
            setDigits(data.samples);
        } catch (error) {
            console.error('Error shuffling digits:', error);
        }
        setIsLoading(false);
    };

    return (
        <div className="max-w-2xl mx-auto p-6">
            <div className="bg-white rounded-lg shadow-lg">
                <div className="p-6">
                    <h2 className="text-2xl font-bold text-center mb-6">MNIST Digit Recognition</h2>
                    <div className="grid grid-cols-3 gap-4 mb-6">
                        {digits.map((digit, i) => (
                            <div key={i} className="flex flex-col items-center p-4 bg-gray-50 rounded-lg">
                                <MNISTDigit imageData={digit.data} />
                                <div className="mt-2 text-lg font-semibold">
                                    Prediction: {digit.prediction}
                                </div>
                            </div>
                        ))}
                    </div>
                    <div className="flex justify-center">
                        <button
                            onClick={handleShuffle}
                            disabled={isLoading}
                            className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600 disabled:opacity-50 flex items-center gap-2"
                        >
                            {isLoading ? 'Loading...' : 'Shuffle Digits'}
                        </button>
                    </div>
                </div>
            </div>
        </div>
    );
};

// Render the app
const root = createRoot(document.getElementById('root'));
root.render(<MNISTGrid />);